#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <filesystem>
#include <map>
#include <memory>
#include <utility>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "cppql-core/database.h"
#include "cppql-typed/typed_table.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "alexandria/property.h"
#include "alexandria/type.h"
#include "alexandria/object_handler/object_handler.h"

namespace alex
{
    class Library;
    using LibraryPtr = std::unique_ptr<Library>;
    using TypeMap    = std::map<std::string, TypePtr>;

    constexpr size_t TypeTableIdIndex               = 0;
    constexpr size_t TypeTableNameIndex             = 1;
    constexpr size_t PropertyTableIdIndex           = 0;
    constexpr size_t PropertyTableNameIndex         = 1;
    constexpr size_t PropertyTableDataTypeIndex     = 2;
    constexpr size_t PropertyTableTypeIndex         = 3;
    constexpr size_t PropertyTableReferenceIndex    = 4;
    constexpr size_t PropertyTableArrayIndex        = 5;
    constexpr size_t PropertyTableBlobIndex         = 6;
    constexpr size_t PropertyTableDefaultValueIndex = 7;
    constexpr size_t MemberTableIdIndex             = 0;
    constexpr size_t MemberTableTypeIndex           = 1;
    constexpr size_t MemberTablePropertyIndex       = 2;

    class Library
    {
    public:
        Library() = delete;

        Library(const Library&) = delete;

        Library(Library&&) = default;

        explicit Library(sql::DatabasePtr db);

        ~Library();

        Library& operator=(const Library&) = delete;

        Library& operator=(Library&&) = default;

        ////////////////////////////////////////////////////////////////
        // Static open/create methods.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Create a new library.
         * \param file Path to library file.
         * \return Library.
         */
        static LibraryPtr create(const std::filesystem::path& file);

        /**
         * \brief Open an existing library.
         * \param file Path to library file.
         * \return Library.
         */
        static LibraryPtr open(const std::filesystem::path& file);

        /**
         * \brief Open an existing library or create one if it does not exist.
         * \param file Path to library file.
         * \return Library and boolean indicating whether a new library was created.
         */
        static std::pair<LibraryPtr, bool> openOrCreate(const std::filesystem::path& file);

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Get the sqlite database.
         * \return Database.
         */
        [[nodiscard]] sql::Database& getDatabase() noexcept;

        /**
         * \brief Get the sqlite database.
         * \return Const database.
         */
        [[nodiscard]] const sql::Database& getDatabase() const noexcept;

        [[nodiscard]] Type& getType(const std::string& name);

        [[nodiscard]] const Type& getType(const std::string& name) const;

        /**
         * \brief Get table containing type definitions.
         * \return Type table.
         */
        [[nodiscard]] TypeTable& getTypeTable() noexcept;

        /**
         * \brief Get table containing type definitions.
         * \return Const type table.
         */
        [[nodiscard]] const TypeTable& getTypeTable() const noexcept;

        /**
         * \brief Get table containing property definitions.
         * \return Property table.
         */
        [[nodiscard]] PropertyTable& getPropertyTable() noexcept;

        /**
         * \brief Get table containing property definitions.
         * \return Const property table.
         */
        [[nodiscard]] const PropertyTable& getPropertyTable() const noexcept;

        /**
         * \brief Get table containing type members.
         * \return Member table.
         */
        [[nodiscard]] MemberTable& getMemberTable() noexcept;

        /**
         * \brief Get table containing type members.
         * \return Const member table.
         */
        [[nodiscard]] const MemberTable& getMemberTable() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Create a new type.
         * \param name Type name.
         * \return Type.
         */
        Type& createType(const std::string& name);

        /**
         * \brief Commit all uncommitted types.
         */
        void commitTypes();

        /**
         * \brief Commit a type. Will also commit all types it depends on.
         * \param type Type to commit.
         */
        void commitType(Type& type);

        ////////////////////////////////////////////////////////////////
        // ...
        ////////////////////////////////////////////////////////////////

        // TODO: Require IdMember to be member to ID.
        // Require Members... to be Member or MemberLists...

        template<typename IdMember, typename... Members>
        [[nodiscard]] auto createObjectHandler(const Type& type) const
        {
            auto f = [&]<typename... Ts>(std::tuple<Ts...>)
            {
                return std::make_unique<ObjectHandler<Ts...>>(type, *database);
            };
            return f(GenerateMemberTuple::get<IdMember, Members...>());
        }

        template<typename IdMember, typename... Members>
        [[nodiscard]] auto createObjectHandler(const std::string& type) const
        {
            return createObjectHandler<IdMember, Members...>(getType(type));
        }

        template<typename T>
        requires requires(T)
        {
            typename T::element_type;
        }
        [[nodiscard]] auto createObjectHandler(const Type& type) const
        {
            return std::make_unique<typename T::element_type>(type, *database);
        }

        /**
         * \brief Write all types and properties as a graph in the DOT format.
         * \param out Ostream.
         */
        void writeGraph(std::ostream& out) const;

    private:
        void readSpecification();

        /**
         * \brief Sqlite database handle.
         */
        sql::DatabasePtr database;

        /**
         * \brief Sqlite table containing type definitions.
         */
        TypeTable typeTable;

        /**
         * \brief Sqlite table containing property definitions.
         */
        PropertyTable propertyTable;

        /**
         * \brief Sqlite table containing type members.
         */
        MemberTable memberTable;

        /**
         * \brief Types.
         */
        TypeMap types;
    };


    ////////////////////////////////////////////////////////////////
    // ObjectHandler type generator.
    ////////////////////////////////////////////////////////////////

    template<typename IdMember, typename... Members>
    using object_handler_t =
      decltype(std::declval<LibraryPtr>()->createObjectHandler<IdMember, Members...>(std::declval<std::string>()));
}  // namespace alex