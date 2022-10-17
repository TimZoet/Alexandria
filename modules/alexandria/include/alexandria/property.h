#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <memory>
#include <string>
#include <variant>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "cppql-core/database.h"
#include "cppql-typed/typed_table.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "alexandria/data_type.h"
#include "alexandria/member_types/instance_id.h"

namespace alex
{
    class Library;
    class Property;
    class Type;
    using PropertyPtr = std::unique_ptr<Property>;
    using TypeTable   = sql::TypedTable<sql::row_id, std::string>;
    using PropertyTable =
      sql::TypedTable<sql::row_id, std::string, std::string, sql::row_id, int32_t, int32_t, int32_t, std::string>;
    using MemberTable = sql::TypedTable<sql::row_id, sql::row_id, sql::row_id>;

    class Property
    {
    public:
        friend class Library;
        friend class Type;

        using default_value_t =
          std::variant<std::nullptr_t, int32_t, uint32_t, int64_t, uint64_t, float, double, std::string>;

        Property(std::string propName, DataType type, bool isReference, bool isArray, bool isBlob);

        Property(sql::row_id propId, std::string propName, DataType type, bool isReference, bool isArray, bool isBlob);

        Property(std::string propName, Type& refType, bool isReference, bool isArray, bool isBlob);

        Property(sql::row_id propId, std::string propName, Type& refType, bool isReference, bool isArray, bool isBlob);

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
         * \brief Returns whether this property was committed to the database.
         * \return True if committed.
         */
        [[nodiscard]] bool isCommitted() const noexcept;

        /**
         * \brief Get internal type ID.
         * \return Type ID.
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
         * \brief Returns whether this property is a reference.
         * \return True if reference.
         */
        [[nodiscard]] bool isReference() const noexcept;

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

        [[nodiscard]] bool isPrimitive() const noexcept;

        [[nodiscard]] bool isType() const noexcept;

        [[nodiscard]] const default_value_t& getDefaultValue() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        void setDefaultValue(int32_t value);

        void setDefaultValue(uint32_t value);

        void setDefaultValue(int64_t value);

        void setDefaultValue(uint64_t value);

        void setDefaultValue(float value);

        void setDefaultValue(double value);

        void setDefaultValue(std::string value);

        ////////////////////////////////////////////////////////////////
        // ...
        ////////////////////////////////////////////////////////////////

        void deleteReferences(InstanceId instId) const;

    private:
        /**
         * \brief Commit property to the database. Also commits all dependencies.
         * \param db Sqlite database.
         * \param typeTable Type table.
         * \param propsTable Property table.
         * \param memTable Member table.
         */
        void commit(sql::Database& db, TypeTable& typeTable, PropertyTable& propsTable, MemberTable& memTable);

        /**
         * \brief Generate columns for instance table and create reference tables.
         * \param db Sqlite database.
         * \param table Instance table.
         * \param prefix Concatenation of all parent property names.
         */
        void generateTables(sql::Database& db, sql::Table& table, const std::string& prefix) const;

        /**
         * \brief Resolve references.
         * \param db Sqlite database.
         * \param table Instance table.
         * \param prefix Concatenation of all parent property names.
         */
        void resolve(sql::Database& db, sql::Table& table, const std::string& prefix);

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Row ID.
         */
        sql::row_id id;

        /**
         * \brief Property name.
         */
        std::string name;

        /**
         * \brief Data type.
         */
        DataType dataType;

        /**
         * \brief Pointer to type. Only set if dataType == Type.
         */
        Type* referenceType;

        /**
         * \brief Indicates property is a reference type.
         */
        bool reference;

        /**
         * \brief Indicates property is an array type.
         */
        bool array;

        /**
         * \brief Indicates property is a blob type.
         */
        bool blob;

        default_value_t defaultValue;

        sql::StatementPtr deleteRefStmt;
    };
}  // namespace alex