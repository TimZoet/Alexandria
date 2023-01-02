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

#include "cppql/include_all.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "alexandria/core/namespace.h"
#include "alexandria/core/property.h"
#include "alexandria/core/type.h"

namespace alex
{
    class Library
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        Library() = delete;

        Library(const Library&) = delete;

        Library(Library&&) noexcept = default;

        explicit Library(sql::DatabasePtr db);

        ~Library() noexcept;

        Library& operator=(const Library&) = delete;

        Library& operator=(Library&&) noexcept = default;

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

        /**
         * \brief Get table containing namespace definitions.
         * \return Namespace table.
         */
        [[nodiscard]] NamespaceTable& getNamespaceTable() noexcept;

        /**
         * \brief Get table containing namespace definitions.
         * \return Namespace table.
         */
        [[nodiscard]] const NamespaceTable& getNamespaceTable() const noexcept;

        /**
         * \brief Get table containing type definitions.
         * \return Type table.
         */
        [[nodiscard]] TypeTable& getTypeTable() noexcept;

        /**
         * \brief Get table containing type definitions.
         * \return Type table.
         */
        [[nodiscard]] const TypeTable& getTypeTable() const noexcept;

        /**
         * \brief Get table containing property definitions.
         * \return Property table.
         */
        [[nodiscard]] PropertyTable& getPropertyTable() noexcept;

        /**
         * \brief Get table containing property definitions.
         * \return Property table.
         */
        [[nodiscard]] const PropertyTable& getPropertyTable() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Namespaces.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Create a new namescape.
         * \param name Unique namespace name.
         * \return Namespace.
         */
        Namespace& createNamespace(const std::string& name);

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
         * \brief Namespaces.
         */
        NamespaceMap namespaces;

        /**
         * \brief Sqlite table containing namespace definitions.
         */
        NamespaceTable namespaceTable;

        /**
         * \brief Sqlite table containing type definitions.
         */
        TypeTable typeTable;

        /**
         * \brief Sqlite table containing property definitions.
         */
        PropertyTable propertyTable;
    };
}  // namespace alex
