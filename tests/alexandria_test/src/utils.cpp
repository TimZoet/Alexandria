#include "alexandria_test/utils.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <filesystem>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "cppql-typed/typed_table.h"
#include "cppql-typed/queries/select.h"

namespace utils
{
    LibraryMember::LibraryMember()
    {
        const auto cwd    = std::filesystem::current_path();
        const auto dbPath = cwd / "lib.db";
        if (std::filesystem::exists(dbPath)) std::filesystem::remove(dbPath);
        library = alex::Library::create(dbPath);
    }

    LibraryMember::~LibraryMember() noexcept
    {
        try
        {
            library.reset();
            const auto cwd    = std::filesystem::current_path();
            const auto dbPath = cwd / "lib.db";
            std::filesystem::remove(dbPath);
        }
        catch (...)
        {
        }
    }

    void LibraryMember::reopen()
    {
        library.reset();
        const auto cwd    = std::filesystem::current_path();
        const auto dbPath = cwd / "lib.db";
        library           = alex::Library::open(dbPath);
    }

    void LibraryMember::checkTypeTables(std::vector<Type>     types,
                                        std::vector<Property> properties,
                                        std::vector<Member>   members)
    {
        // Get type(s) from internal table.
        auto& typeTable = library->getTypeTable();
        auto  select1   = typeTable.select<Type, alex::TypeTableIdIndex, alex::TypeTableNameIndex>(
          typeTable.col<alex::TypeTableIdIndex>() >= 0, true);
        std::vector<Type> _types(select1.begin(), select1.end());

        // Get prop(s) from internal table.
        auto& propTable = library->getPropertyTable();
        auto  select2 =
          propTable.select<Property,
                           alex::PropertyTableIdIndex,
                           alex::PropertyTableNameIndex,
                           alex::PropertyTableDataTypeIndex,
                           alex::PropertyTableTypeIndex,
                           alex::PropertyTableReferenceIndex,
                           alex::PropertyTableArrayIndex,
                           alex::PropertyTableBlobIndex>(propTable.col<alex::PropertyTableIdIndex>() >= 0, true);
        std::vector<Property> _properties(select2.begin(), select2.end());

        // Get member(s) from internal table.
        auto& memberTable = library->getMemberTable();
        auto  select3 =
          memberTable
            .select<Member, alex::MemberTableIdIndex, alex::MemberTableTypeIndex, alex::MemberTablePropertyIndex>(
              memberTable.col<alex::MemberTableTypeIndex>() >= 0, true);
        std::vector<Member> _members(select3.begin(), select3.end());

        // TODO: Once the select statements allow ordering, order directly by ID ASC instead of doing it here.
        // Will also allow removing ID from Type, Property and Member structs.
        std::sort(_types.begin(), _types.end(), [](const Type& lhs, const Type& rhs) { return lhs.id < rhs.id; });
        std::sort(_properties.begin(), _properties.end(), [](const Property& lhs, const Property& rhs) {
            return lhs.id < rhs.id;
        });
        std::sort(
          _members.begin(), _members.end(), [](const Member& lhs, const Member& rhs) { return lhs.id < rhs.id; });
        

        // Check types of lists.
        compareEQ(types.size(), _types.size()).fatal("Number of types does not match");
        compareEQ(properties.size(), _properties.size()).fatal("Number of properties does not match");
        compareEQ(members.size(), _members.size()).fatal("Number of members does not match");

        // Compare types.
        for (size_t i = 0; i < types.size(); i++) { compareEQ(types[i].name, _types[i].name); }

        // Compare properties.
        for (size_t i = 0; i < properties.size(); i++)
        {
            compareEQ(properties[i].name, _properties[i].name);
            compareEQ(properties[i].dataType, _properties[i].dataType);
            compareEQ(properties[i].nestedType, _properties[i].nestedType);
            compareEQ(properties[i].isReference, _properties[i].isReference);
            compareEQ(properties[i].isArray, _properties[i].isArray);
            compareEQ(properties[i].isBlob, _properties[i].isBlob);
        }

        // Compare members.
        for (size_t i = 0; i < members.size(); i++)
        {
            compareEQ(members[i].type, _members[i].type);
            compareEQ(members[i].prop, _members[i].prop);
        }
    }
}  // namespace utils