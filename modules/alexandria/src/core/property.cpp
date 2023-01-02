#include "alexandria/core/property.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <format>
#include <stdexcept>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "cppql/include_all.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "alexandria/core/library.h"
#include "alexandria/core/namespace.h"
#include "alexandria/core/type.h"

namespace alex
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    Property::Property(Type&             t,
                       const sql::row_id propId,
                       std::string       propName,
                       const DataType    type,
                       Type*             refType,
                       const bool        isArray,
                       const bool        isBlob) :
        type(&t),
        id(propId),
        name(std::move(propName)),
        dataType(type),
        referenceType(refType),
        array(isArray),
        blob(isBlob)
    {
    }

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    Type& Property::getType() noexcept { return *type; }

    const Type& Property::getType() const noexcept { return *type; }

    bool Property::isCommitted() const noexcept { return id != -1; }

    sql::row_id Property::getId() const noexcept { return id; }

    const std::string& Property::getName() const noexcept { return name; }

    DataType Property::getDataType() const noexcept { return dataType; }

    Type* Property::getReferenceType() noexcept { return referenceType; }

    const Type* Property::getReferenceType() const noexcept { return referenceType; }

    bool Property::isArray() const noexcept { return array; }

    bool Property::isBlob() const noexcept { return blob; }

    auto Property::getDefaultValue() const noexcept -> const default_value_t& { return defaultValue; }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void Property::setDefaultValue(const int32_t value)
    {
        requireNotCommitted();
        if (dataType != DataType::Int32)
            throw std::runtime_error("Cannot set int32 default value if DataType != int32");
        defaultValue = value;
    }

    void Property::setDefaultValue(const uint32_t value)
    {
        requireNotCommitted();
        if (dataType != DataType::Uint32)
            throw std::runtime_error("Cannot set uint32 default value if DataType != uint32");
        defaultValue = value;
    }

    void Property::setDefaultValue(const int64_t value)
    {
        requireNotCommitted();
        if (dataType != DataType::Int64)
            throw std::runtime_error("Cannot set int64 default value if DataType != int64");
        defaultValue = value;
    }

    void Property::setDefaultValue(const uint64_t value)
    {
        requireNotCommitted();
        if (dataType != DataType::Uint64)
            throw std::runtime_error("Cannot set uint64 default value if DataType != uint64");
        defaultValue = value;
    }

    void Property::setDefaultValue(const float value)
    {
        requireNotCommitted();
        if (dataType != DataType::Float)
            throw std::runtime_error("Cannot set float default value if DataType != float");
        defaultValue = value;
    }

    void Property::setDefaultValue(const double value)
    {
        requireNotCommitted();
        if (dataType != DataType::Double)
            throw std::runtime_error("Cannot set double default value if DataType != double");
        defaultValue = value;
    }

    void Property::setDefaultValue(std::string value)
    {
        requireNotCommitted();
        if (dataType != DataType::String)
            throw std::runtime_error("Cannot set string default value if DataType != String");
        defaultValue = std::move(value);
    }

    ////////////////////////////////////////////////////////////////
    // ...
    ////////////////////////////////////////////////////////////////

    void Property::requireNotCommitted() const
    {
        if (isCommitted())
            throw std::runtime_error(std::format("Property {}::{} was already committed.", type->getName(), name));
    }

    void Property::requireCommitted() const
    {
        if (!isCommitted())
            throw std::runtime_error(std::format("Property {}::{} was not yet committed.", type->getName(), name));
    }

    void Property::requireReferencedTypesCommitted() const
    {
        if (dataType == DataType::Reference || dataType == DataType::Nested)
            if (!referenceType->isCommitted())
                throw std::runtime_error(
                  std::format("Property {}::{} cannot be committed because it references uncommitted type {}::{}.",
                              type->getName(),
                              name,
                              referenceType->getNamespace().getName(),
                              referenceType->getName()));
    }

    void Property::commit()
    {
        requireNotCommitted();

        // TODO: Format default value as string.
        //std::string defaultVal;
        //std::visit(
        //  [&defaultVal](auto&& arg) {
        //      using T = std::decay_t<decltype(arg)>;
        //      if constexpr (std::same_as<T, std::nullptr_t>)
        //          defaultVal = "NULL";
        //      else if constexpr (std::integral<T> || std::floating_point<T>)
        //          defaultVal = std::to_string(arg);
        //      else if constexpr (std::same_as<T, std::string>)
        //          defaultVal = "'" + arg + "'";
        //      else
        //          constexpr_static_assert();
        //  },
        //  defaultValue);

        const auto& nameSpace     = type->getNamespace();
        const auto& library       = nameSpace.getLibrary();
        const auto& db            = library.getDatabase();
        const auto& propertyTable = library.getPropertyTable();

        // Add property to table.
        auto       insert     = propertyTable.insert().compile();
        const auto typeString = toString(dataType);
        insert(nullptr,
               type->getId(),
               sql::toStaticText(name),
               sql::toStaticText(typeString),
               referenceType ? std::make_optional(referenceType->getId()) : std::optional<sql::row_id>(),
               isArray() ? 1 : 0,
               isBlob() ? 1 : 0);

        // Set ID.
        id = db.getLastInsertRowId();
    }

    void Property::generate(sql::Table& instanceTable, const std::string& prefix) const
    {
        auto& db = instanceTable.getDatabase();

        if (isPrimitiveDataType(dataType) || dataType == DataType::String || dataType == DataType::Blob)
        {
            if (array)
            {
                // Create a separate table with a foreign key to the instance table of referenceType and a value column.
                auto& arrayTable = db.createTable(instanceTable.getName() + "_" + prefix + name);
                arrayTable.createColumn("id", sql::Column::Type::Int).primaryKey(true);
                arrayTable.createColumn("instance", sql::Column::Type::Text)
                  .foreignKey(instanceTable.getColumn("uuid"), sql::ForeignKeyAction::Cascade)
                  .notNull();
                arrayTable.createColumn("value", toColumnType(dataType));
                arrayTable.commit();
            }
            else if (blob)
            {
                // Add a column with blob type.
                instanceTable.createColumn(prefix + name, sql::Column::Type::Blob);
            }
            else
            {
                // Add a column with primitive/string/blob type.
                instanceTable.createColumn(prefix + name, toColumnType(dataType));
            }
        }
        else if (dataType == DataType::Nested)
        {
            // Recurse.
            const auto prefix2 = prefix + "_" + name;
            for (const auto& prop : referenceType->getProperties()) prop->generate(instanceTable, prefix2);
        }
        else if (dataType == DataType::Reference)
        {
            auto& refColumn = db.getTable(referenceType->getInstanceTableName()).getColumn("uuid");

            if (array)
            {
                // Create a separate table with foreign keys to the instance table of this type and referenceType.
                auto& arrayTable = db.createTable(instanceTable.getName() + "_" + prefix + name);
                arrayTable.createColumn("id", sql::Column::Type::Int).primaryKey(true);
                arrayTable.createColumn("instance", sql::Column::Type::Text)
                  .foreignKey(instanceTable.getColumn("uuid"), sql::ForeignKeyAction::Cascade)
                  .notNull();
                arrayTable.createColumn("value", sql::Column::Type::Text)
                  .foreignKey(refColumn, sql::ForeignKeyAction::Cascade);
                arrayTable.commit();
            }
            else
            {
                // Add a column with a foreign key to the instance table of referenceType.
                instanceTable.createColumn(prefix + name, sql::Column::Type::Text)
                  .foreignKey(refColumn, sql::ForeignKeyAction::SetNull);
            }
        }
    }
}  // namespace alex
