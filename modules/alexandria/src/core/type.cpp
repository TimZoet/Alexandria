#include "alexandria/core/type.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <algorithm>
#include <format>
#include <stdexcept>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "cppql/include_all.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include "alexandria/core/library.h"
#include "alexandria/core/namespace.h"

namespace alex
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    Type::Type(Namespace& ns, std::string typeName) : nameSpace(&ns), name(std::move(typeName)) {}

    Type::Type(const sql::row_id rid, Namespace& ns, std::string typeName, const bool isInst) :
        id(rid), nameSpace(&ns), name(std::move(typeName)), isInstance(isInst)
    {
    }

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    Namespace& Type::getNamespace() noexcept { return *nameSpace; }

    const Namespace& Type::getNamespace() const noexcept { return *nameSpace; }

    bool Type::isCommitted() const noexcept { return id != -1; }

    sql::row_id Type::getId() const noexcept { return id; }

    const std::string& Type::getName() const noexcept { return name; }

    std::string Type::getInstanceTableName() const noexcept { return nameSpace->getName() + "_" + name; }

    sql::Table& Type::getInstanceTable() const
    {
        requireCommitted();
        return nameSpace->getLibrary().getDatabase().getTable(getInstanceTableName());
    }

    const PropertyList& Type::getProperties() const noexcept { return properties; }

    ////////////////////////////////////////////////////////////////
    // Properties.
    ////////////////////////////////////////////////////////////////

    Property& Type::createPrimitiveProperty(const std::string& propName, const DataType dataType)
    {
        requireNotCommitted();

        if (!isPrimitiveDataType(dataType)) throw std::runtime_error("dataType is not a primitive type");

        return createProperty(propName, dataType, nullptr, false, false);
    }

    Property& Type::createPrimitiveArrayProperty(const std::string& propName, const DataType dataType)
    {
        requireNotCommitted();

        if (!isPrimitiveDataType(dataType)) throw std::runtime_error("dataType is not a primitive type");

        return createProperty(propName, dataType, nullptr, true, false);
    }

    Property& Type::createPrimitiveBlobProperty(const std::string& propName, const DataType dataType)
    {
        requireNotCommitted();

        if (!isPrimitiveDataType(dataType)) throw std::runtime_error("dataType is not a primitive type");

        return createProperty(propName, dataType, nullptr, false, true);
    }

    Property& Type::createStringProperty(const std::string& propName)
    {
        requireNotCommitted();

        return createProperty(propName, DataType::String, nullptr, false, false);
    }

    Property& Type::createStringArrayProperty(const std::string& propName)
    {
        requireNotCommitted();

        return createProperty(propName, DataType::String, nullptr, true, false);
    }

    Property& Type::createBlobProperty(const std::string& propName)
    {
        requireNotCommitted();

        return createProperty(propName, DataType::Blob, nullptr, false, false);
    }

    Property& Type::createBlobArrayProperty(const std::string& propName)
    {
        requireNotCommitted();

        return createProperty(propName, DataType::Blob, nullptr, true, false);
    }

    Property& Type::createReferenceProperty(const std::string& propName, Type& refType)
    {
        requireNotCommitted();

        return createProperty(propName, DataType::Reference, &refType, false, false);
    }

    Property& Type::createReferenceArrayProperty(const std::string& propName, Type& refType)
    {
        requireNotCommitted();
        // TODO: Verify that refType.isInstance == true. Same for createReferenceProperty.

        return createProperty(propName, DataType::Reference, &refType, true, false);
    }

    Property& Type::createNestedTypeProperty(const std::string& propName, Type& nestedType)
    {
        requireNotCommitted();

        return createProperty(propName, DataType::Nested, &nestedType, false, false);
    }

    std::vector<sql::Table*> Type::getPrimitiveArrayTables() const
    {
        requireCommitted();

        std::vector<sql::Table*> tables;
        for (const auto& prop : properties) prop->getPrimitiveArrayTables(tables, getInstanceTable(), "");
        return tables;
    }

    std::vector<sql::Table*> Type::getBlobArrayTables() const
    {
        requireCommitted();

        std::vector<sql::Table*> tables;
        for (const auto& prop : properties) prop->getBlobArrayTables(tables, getInstanceTable(), "");
        return tables;
    }

    std::vector<sql::Table*> Type::getReferenceArrayTables() const
    {
        requireCommitted();

        std::vector<sql::Table*> tables;
        for (const auto& prop : properties) prop->getReferenceArrayTables(tables, getInstanceTable(), "");
        return tables;
    }

    ////////////////////////////////////////////////////////////////
    // Commit.
    ////////////////////////////////////////////////////////////////

    void Type::requireNotCommitted() const
    {
        if (isCommitted())
            throw std::runtime_error(std::format("Type {}::{} was already committed.", nameSpace->getName(), name));
    }

    void Type::requireCommitted() const
    {
        if (!isCommitted())
            throw std::runtime_error(std::format("Type {}::{} was not yet committed.", nameSpace->getName(), name));
    }

    void Type::commit()
    {
        requireNotCommitted();

        if (properties.empty())
            throw std::runtime_error(
              std::format("Type {}::{} cannot be committed. It has no properties.", nameSpace->getName(), name));

        for (const auto& prop : properties) prop->requireReferencedTypesCommitted();

        auto&       library   = nameSpace->getLibrary();
        auto&       db        = library.getDatabase();

        try
        {
            auto transaction = db.beginTransaction(sql::Transaction::Type::Deferred);

            // Add type to table and retrieve ID.
            library.getTypeTableInsert()(nullptr, nameSpace->getId(), sql::toStaticText(name), isInstance ? 1 : 0);
            id = db.getLastInsertRowId();

            // Commit properties.
            for (const auto& prop : properties) prop->commit();

            // Generate tables.
            if (isInstance) generate();

            transaction.commit();
        }
        catch (const std::exception&)
        {
            id = -1;
            for (const auto& prop : properties) prop->id = -1;
            throw;
        }
    }

    ////////////////////////////////////////////////////////////////
    // Private methods.
    ////////////////////////////////////////////////////////////////

    void Type::generate() const
    {
        auto& library = nameSpace->getLibrary();
        auto& db      = library.getDatabase();

        // Create instance table.
        auto& instanceTable = db.createTable(getInstanceTableName());
        instanceTable.createColumn("id", sql::Column::Type::Int).primaryKey(true);
        instanceTable.createColumn("uuid", sql::Column::Type::Text).unique();  // .primaryKey().unique().notNull();

        // Add columns and reference tables for all properties.
        for (const auto& prop : properties) prop->generate(instanceTable, "");

        // Commit instance table.
        instanceTable.commit();
    }

    Property& Type::createProperty(
      const std::string& propName, const DataType dataType, Type* refType, const bool isArray, const bool isBlob)
    {
        //if (properties.contains(propName)) throw std::runtime_error("A property with this name already exists");
        // TODO: Also check for other forbidden names:
        // Type names such as float, double, int32 etc.
        // ...

        return *properties.emplace_back(
          std::make_unique<Property>(*this, -1, propName, dataType, refType, isArray, isBlob));
    }

    void Type::addProperty(PropertyPtr prop) { properties.emplace_back(std::move(prop)); }
}  // namespace alex
