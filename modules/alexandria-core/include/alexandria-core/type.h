#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <vector>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "cppql/include_all.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "alexandria-core/fwd.h"
#include "alexandria-core/type_layout.h"

namespace alex
{
    class Type
    {
    public:
        friend class Library;
        friend class TypeLayout;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        Type() = default;

        Type(const Type&) = delete;

        Type(Type&&) noexcept = default;

        ~Type() noexcept = default;

        Type& operator=(const Type&) = delete;

        Type& operator=(Type&&) noexcept = default;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Get internal type ID.
         * \return Type ID.
         */
        [[nodiscard]] sql::row_id getId() const noexcept;

        /**
         * \brief Get the type layout.
         * \return Type layout.
         */
        [[nodiscard]] const TypeLayout& getLayout() const noexcept;

        /**
         * \brief Get type name.
         * \return Type name.
         */
        [[nodiscard]] const std::string& getName() const noexcept;

        /**
         * \brief Whether instances of this type can be created, or if it is only to be used as a nested type.
         * If false, no tables are generated for this type.
         * \return Instantiable.
         */
        [[nodiscard]] TypeLayout::Instantiable isInstantiable() const noexcept;

        /**
         * \brief Get namespace this type is in.
         * \return Namespace.
         */
        [[nodiscard]] Namespace& getNamespace() noexcept;

        /**
         * \brief Get namespace this type is in.
         * \return Namespace.
         */
        [[nodiscard]] const Namespace& getNamespace() const noexcept;

        /**
         * \brief Get the instance table that was generated for this type.
         * \return Table.
         */
        [[nodiscard]] sql::Table& getInstanceTable() const;

        /**
         * \brief Get the primitive array tables that were generated for this type.
         * \return List of tables.
         */
        [[nodiscard]] const std::vector<sql::Table*>& getPrimitiveArrayTables() const;

        /**
         * \brief Get the blob array tables that were generated for this type.
         * \return List of tables.
         */
        [[nodiscard]] const std::vector<sql::Table*>& getBlobArrayTables() const;

        /**
         * \brief Get the reference array tables that were generated for this type.
         * \return List of tables.
         */
        [[nodiscard]] const std::vector<sql::Table*>& getReferenceArrayTables() const;

    private:
        /**
         * \brief Row ID.
         */
        sql::row_id id = -1;

        /**
         * \brief Row IDs of all properties.
         */
        std::vector<sql::row_id> propertyIds;

        /**
         * \brief Unique (within a namespace) type name.
         */
        std::string name;

        /**
         * \brief Instantiability. If false, type cannot be instantiated because no tables were generated for it.
         */
        TypeLayout::Instantiable instantiable = TypeLayout::Instantiable::True;

        /**
         * \brief Pointer to type layout.
         */
        TypeLayoutPtr typeLayout;

        /**
         * \brief Namespace.
         */
        Namespace* nameSpace = nullptr;

        struct
        {
            /**
             * \brief Instance table that was generated for this type.
             */
            sql::Table* instance = nullptr;

            /**
             * \brief Primitive array tables that were generated for this type.
             */
            std::vector<sql::Table*> primitiveArrays = {};

            /**
             * \brief Blob array tables that were generated for this type.
             */
            std::vector<sql::Table*> blobArrays = {};

            /**
             * \brief Reference array tables that were generated for this type.
             */
            std::vector<sql::Table*> referenceArrays = {};
        } tables;
    };
}  // namespace alex
