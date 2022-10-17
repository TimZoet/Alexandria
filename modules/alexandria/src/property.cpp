#include "alexandria/property.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <format>
#include <stdexcept>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "cppql-typed/queries/insert.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "alexandria/type.h"

namespace alex
{
    Property::Property(
      std::string propName, const DataType type, const bool isReference, const bool isArray, const bool isBlob) :
        id(-1),
        name(std::move(propName)),
        dataType(type),
        referenceType(nullptr),
        reference(isReference),
        array(isArray),
        blob(isBlob)
    {
    }

    Property::Property(const sql::row_id propId,
                       std::string       propName,
                       const DataType    type,
                       const bool        isReference,
                       const bool        isArray,
                       const bool        isBlob) :
        id(propId),
        name(std::move(propName)),
        dataType(type),
        referenceType(nullptr),
        reference(isReference),
        array(isArray),
        blob(isBlob)
    {
    }

    Property::Property(
      std::string propName, Type& refType, const bool isReference, const bool isArray, const bool isBlob) :
        id(-1),
        name(std::move(propName)),
        dataType(DataType::Type),
        referenceType(&refType),
        reference(isReference),
        array(isArray),
        blob(isBlob)
    {
    }

    Property::Property(const sql::row_id propId,
                       std::string       propName,
                       Type&             refType,
                       const bool        isReference,
                       const bool        isArray,
                       const bool        isBlob) :
        id(propId),
        name(std::move(propName)),
        dataType(DataType::Type),
        referenceType(&refType),
        reference(isReference),
        array(isArray),
        blob(isBlob)
    {
    }

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    bool Property::isCommitted() const noexcept { return id != -1; }

    sql::row_id Property::getId() const noexcept { return id; }

    const std::string& Property::getName() const noexcept { return name; }

    DataType Property::getDataType() const noexcept { return dataType; }

    Type* Property::getReferenceType() noexcept { return referenceType; }

    const Type* Property::getReferenceType() const noexcept { return referenceType; }

    bool Property::isReference() const noexcept { return reference; }

    bool Property::isArray() const noexcept { return array; }

    bool Property::isBlob() const noexcept { return blob; }

    bool Property::isPrimitive() const noexcept { return dataType != DataType::Blob && dataType != DataType::Type; }

    bool Property::isType() const noexcept { return dataType == DataType::Type; }

    auto Property::getDefaultValue() const noexcept -> const default_value_t& { return defaultValue; }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void Property::setDefaultValue(const int32_t value)
    {
        if (isCommitted()) throw std::runtime_error("Property was already committed");
        if (dataType != DataType::Int32)
            throw std::runtime_error("Cannot set int32 default value if DataType != int32");
        defaultValue = value;
    }

    void Property::setDefaultValue(const uint32_t value)
    {
        if (isCommitted()) throw std::runtime_error("Property was already committed");
        if (dataType != DataType::Uint32)
            throw std::runtime_error("Cannot set uint32 default value if DataType != uint32");
        defaultValue = value;
    }

    void Property::setDefaultValue(const int64_t value)
    {
        if (isCommitted()) throw std::runtime_error("Property was already committed");
        if (dataType != DataType::Int64)
            throw std::runtime_error("Cannot set int64 default value if DataType != int64");
        defaultValue = value;
    }

    void Property::setDefaultValue(const uint64_t value)
    {
        if (isCommitted()) throw std::runtime_error("Property was already committed");
        if (dataType != DataType::Uint64)
            throw std::runtime_error("Cannot set uint64 default value if DataType != uint64");
        defaultValue = value;
    }

    void Property::setDefaultValue(const float value)
    {
        if (isCommitted()) throw std::runtime_error("Property was already committed");
        if (dataType != DataType::Float)
            throw std::runtime_error("Cannot set float default value if DataType != float");
        defaultValue = value;
    }

    void Property::setDefaultValue(const double value)
    {
        if (isCommitted()) throw std::runtime_error("Property was already committed");
        if (dataType != DataType::Double)
            throw std::runtime_error("Cannot set double default value if DataType != double");
        defaultValue = value;
    }

    void Property::setDefaultValue(std::string value)
    {
        if (isCommitted()) throw std::runtime_error("Property was already committed");
        if (dataType != DataType::String)
            throw std::runtime_error("Cannot set string default value if DataType != String");
        defaultValue = std::move(value);
    }

    ////////////////////////////////////////////////////////////////
    // ...
    ////////////////////////////////////////////////////////////////

    void Property::deleteReferences(const InstanceId instId) const
    {
        if (deleteRefStmt)
        {
            const auto res =
              deleteRefStmt->bind<InstanceId::type>(0, instId) | deleteRefStmt->step() | deleteRefStmt->reset();
            if (!res) throw std::runtime_error("");
        }
    }

    void Property::commit(sql::Database& db, TypeTable& typeTable, PropertyTable& propsTable, MemberTable& memTable)
    {
        if (isCommitted()) throw std::runtime_error("Property was already committed");

        // If this property references a type commit that type first.
        if (getReferenceType() && !getReferenceType()->isCommitted())
            getReferenceType()->commit(db, typeTable, propsTable, memTable);

        // Format default value as string.
        std::string defaultVal;
        std::visit(
          [&defaultVal](auto&& arg) {
              using T = std::decay_t<decltype(arg)>;
              if constexpr (std::same_as<T, std::nullptr_t>)
                  defaultVal = "NULL";
              else if constexpr (std::integral<T> || std::floating_point<T>)
                  defaultVal = std::to_string(arg);
              else if constexpr (std::same_as<T, std::string>)
                  defaultVal = "'" + arg + "'";
              else
                  constexpr_static_assert();
          },
          defaultValue);

        // Add property to table.
        auto       insert     = propsTable.insert<0, 1, 2, 3, 4, 5, 6, 7>();
        const auto typeString = toString(getDataType());
        insert(nullptr,
               sql::toStaticText(name),
               sql::toStaticText(typeString),
               referenceType ? std::make_optional(referenceType->getId()) : std::optional<sql::row_id>(),
               isReference() ? 1 : 0,
               isArray() ? 1 : 0,
               isBlob() ? 1 : 0,
               sql::toStaticText(defaultVal));

        // Set ID.
        id = db.getLastInsertRowId();
    }

    void Property::generateTables(sql::Database& db, sql::Table& table, const std::string& prefix) const
    {
        if (dataType == DataType::Type)
        {
            if (isArray())
            {
                // Create reference table.
                auto& refTable = db.createTable(table.getName() + "_" + prefix + getName());
                refTable.createColumn("id", sql::Column::Type::Int)
                  .setPrimaryKey(true)
                  .setAutoIncrement(true)
                  .setNotNull(true);

                // Create FK to current type.
                refTable.createColumn("parent", table.getColumn("id")).setNotNull(true);

                // Create FK to member type.
                refTable.createColumn("child", db.getTable(referenceType->getName()).getColumn("id")).setNotNull(true);

                // Commit table.
                refTable.commit();
            }
            // Create FK column to member type.
            else if (isReference())
            {
                table.createColumn(prefix + getName(), db.getTable(referenceType->getName()).getColumn("id"));
            }
            // Member type must be expanded to columns.
            else
                referenceType->generateTables(db, table, prefix + getName() + "_");
        }
        // Create blob column or reference table.
        else if (dataType == DataType::Blob)
        {
            if (isArray() && isReference())
            {
                // Create reference table.
                auto& refTable = db.createTable(table.getName() + "_" + prefix + getName());
                refTable.createColumn("id", sql::Column::Type::Int)
                  .setPrimaryKey(true)
                  .setAutoIncrement(true)
                  .setNotNull(true);

                // Create FK to current type.
                refTable.createColumn("parent", table.getColumn("id")).setNotNull(true);

                // Create blob column.
                refTable.createColumn("value", sql::Column::Type::Blob);

                // Commit table.
                refTable.commit();
            }
            else
                table.createColumn(prefix + getName(), sql::Column::Type::Blob);
        }
        // Create primitive type column or reference table.
        else
        {
            if (isArray() && isReference())
            {
                // Create reference table.
                auto& refTable = db.createTable(table.getName() + "_" + prefix + getName());
                refTable.createColumn("id", sql::Column::Type::Int)
                  .setPrimaryKey(true)
                  .setAutoIncrement(true)
                  .setNotNull(true);

                // Create FK to current type.
                refTable.createColumn("parent", table.getColumn("id")).setNotNull(true);

                // Create value column.
                refTable.createColumn("value", toColumnType(dataType)).setNotNull(true);

                // Commit table.
                refTable.commit();
            }
            else if (isArray() && isBlob()) { table.createColumn(prefix + getName(), sql::Column::Type::Blob); }
            else
            {
                auto& col = table.createColumn(prefix + getName(), toColumnType(dataType));

                if (!std::holds_alternative<std::nullptr_t>(defaultValue))
                {
                    switch (dataType)
                    {
                    case DataType::Int32: col.setDefaultValue(std::get<int32_t>(defaultValue)); break;
                    case DataType::Uint32:
                        col.setDefaultValue(static_cast<int64_t>(std::get<uint32_t>(defaultValue)));
                        break;
                    case DataType::Int64: col.setDefaultValue(std::get<int64_t>(defaultValue)); break;
                    case DataType::Uint64:
                        col.setDefaultValue(static_cast<int64_t>(std::get<uint64_t>(defaultValue)));
                        break;
                    case DataType::Float: col.setDefaultValue(std::get<float>(defaultValue)); break;
                    case DataType::Double: col.setDefaultValue(std::get<double>(defaultValue)); break;
                    case DataType::String: col.setDefaultValue(std::get<std::string>(defaultValue)); break;
                    case DataType::Blob:
                    case DataType::Type: break;
                    }
                }
            }
        }
    }

    void Property::resolve(sql::Database& db, sql::Table& table, const std::string& prefix)
    {
        // Any non-reference and non-recursive properties can be ignored.
        if (dataType != DataType::Type) return;

        // Property is a reference array.
        if (isArray())
        {
            // Register this property with the type it references.
            referenceType->addReference(*this);

            // Create statement to delete all rows from reference table that point to a specific instance.
            auto sql = std::format("DELETE FROM {0} WHERE child = ?1;", table.getName() + "_" + prefix + getName());
            deleteRefStmt = std::make_unique<sql::Statement>(db, std::move(sql), true);
        }
        // Property is a reference.
        else if (isReference())
        {
            // Register this property with the type it references.
            referenceType->addReference(*this);

            // Create statement to clear column in instance table for all rows that point to a specific instance.
            auto sql = std::format("UPDATE {0} SET {1} = NULL WHERE {1} = ?1;", table.getName(), prefix + getName());
            deleteRefStmt = std::make_unique<sql::Statement>(db, std::move(sql), true);
        }
        // Property is a nested type.
        else
            referenceType->resolve(db, table, prefix + getName() + "_");
    }

}  // namespace alex