#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <string>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "cppql/include_all.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "alexandria/core/data_type.h"
#include "alexandria/member_types/instance_id.h"

namespace alex
{
    class Property
    {
    public:
        friend class Library;
        friend class Type;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        Property(
          Type& t, sql::row_id propId, std::string propName, DataType type, Type* refType, bool isArray, bool isBlob);

        Property() = delete;

        Property(const Property&) = delete;

        Property(Property&&) = default;

        ~Property() = default;

        Property& operator=(const Property&) = delete;

        Property& operator=(Property&&) = default;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Get type this property is a part of.
         * \return Type.
         */
        [[nodiscard]] Type& getType() noexcept;

        /**
         * \brief Get type this property is a part of.
         * \return Type.
         */
        [[nodiscard]] const Type& getType() const noexcept;

        /**
         * \brief Returns whether this property was committed to the database.
         * \return True if committed.
         */
        [[nodiscard]] bool isCommitted() const noexcept;

        /**
         * \brief Get internal property ID.
         * \return Property ID.
         */
        [[nodiscard]] sql::row_id getId() const noexcept;

        /**
         * \brief Get property name.
         * \return Property name.
         */
        [[nodiscard]] const std::string& getName() const noexcept;

        /**
         * \brief Get data type.
         * \return DataType.
         */
        [[nodiscard]] DataType getDataType() const noexcept;

        /**
         * \brief Get type referenced by this property.
         * \return Pointer to type. Returns null if there is no type.
         */
        [[nodiscard]] Type* getReferenceType() noexcept;

        /**
         * \brief Get type.
         * \return Pointer to type. Returns null if there is no type.
         */
        [[nodiscard]] const Type* getReferenceType() const noexcept;

        /**
         * \brief Returns whether this property is an array.
         * \return True if array.
         */
        [[nodiscard]] bool isArray() const noexcept;

        /**
         * \brief Returns whether this property is a blob.
         * \return True if blob.
         */
        [[nodiscard]] bool isBlob() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Commit.
        ////////////////////////////////////////////////////////////////

        void requireNotCommitted() const;

        void requireCommitted() const;

        void requireReferencedTypesCommitted() const;

    private:
        /**
         * \brief Commit this property to the library. Inserts entries into the property table.
         */
        void commit();

        void generate(sql::Table& instanceTable, const std::string& prefix) const;

        void getPrimitiveArrayTables(std::vector<sql::Table*>& tables,
                                     const sql::Table&         instanceTable,
                                     const std::string&        prefix) const;

        void getBlobArrayTables(std::vector<sql::Table*>& tables,
                                const sql::Table&         instanceTable,
                                const std::string&        prefix) const;

        void getReferenceArrayTables(std::vector<sql::Table*>& tables,
                                     const sql::Table&         instanceTable,
                                     const std::string&        prefix) const;

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        Type* type = nullptr;

        /**
         * \brief Row ID.
         */
        sql::row_id id = -1;

        /**
         * \brief Property name.
         */
        std::string name;

        /**
         * \brief Data type.
         */
        DataType dataType;

        /**
         * \brief Pointer to type. Only set if dataType == Reference or Nested.
         */
        Type* referenceType;

        /**
         * \brief Indicates property is an array type.
         */
        bool array;

        /**
         * \brief Indicates property is a blob type.
         */
        bool blob;
    };
}  // namespace alex