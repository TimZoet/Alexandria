#include "alexandria_testutils/utils.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <filesystem>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "cppql/typed/typed_table.h"

namespace utils
{
    LibraryMember::LibraryMember(const bool inMem) : inMemory(inMem)
    {
        if (inMemory)
            library = alex::Library::create("");
        else
        {
            const auto cwd    = std::filesystem::current_path();
            const auto dbPath = cwd / "lib.db";
            if (exists(dbPath)) std::filesystem::remove(dbPath);
            library = alex::Library::create(dbPath);
        }

        nameSpace = &library->createNamespace("main");
    }

    LibraryMember::~LibraryMember() noexcept
    {
        try
        {
            library.reset();
            if (!inMemory)
            {
                const auto cwd    = std::filesystem::current_path();
                const auto dbPath = cwd / "lib.db";
                std::filesystem::remove(dbPath);
            }
        }
        catch (...)
        {
        }
    }

    void LibraryMember::reopen()
    {
        library.reset();
        if (!inMemory)
        {
            const auto cwd    = std::filesystem::current_path();
            const auto dbPath = cwd / "lib.db";
            library           = alex::Library::open(dbPath);
        }
        else
            throw std::runtime_error("Cannot reopen in-memory library.");
    }

    void LibraryMember::checkTypeTables(const std::vector<alex::NamespaceRow>& namespaces,
                                        const std::vector<alex::TypeRow>&      types,
                                        const std::vector<alex::PropertyRow>&  properties,
                                        const std::vector<alex::TableRow>&     tables)
    {
        {
            const auto& table  = library->getNamespaceTable();
            auto        select = table.selectAs<alex::NamespaceRow>().orderBy(ascending(table.col<0>())).compile();
            const std::vector<alex::NamespaceRow> rows(select.begin(), select.end());
            compareEQ(namespaces, rows);
        }
        {
            const auto& table  = library->getTypeTable();
            auto        select = table.selectAs<alex::TypeRow>().orderBy(ascending(table.col<0>())).compile();
            const std::vector<alex::TypeRow> rows(select.begin(), select.end());
            compareEQ(types, rows);
        }
        {
            const auto& table  = library->getPropertyTable();
            auto        select = table.selectAs<alex::PropertyRow>().orderBy(ascending(table.col<0>())).compile();
            const std::vector<alex::PropertyRow> rows(select.begin(), select.end());
            compareEQ(properties, rows);
        }
        {
            const auto& table  = library->getGeneratedTablesTable();
            auto        select = table.selectAs<alex::TableRow>().orderBy(ascending(table.col<0>())).compile();
            const std::vector<alex::TableRow> rows(select.begin(), select.end());
            compareEQ(tables, rows);
        }
    }
}  // namespace utils