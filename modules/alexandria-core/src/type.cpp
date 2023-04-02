#include "alexandria-core/type.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <algorithm>
#include <format>
#include <regex>
#include <stdexcept>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "cppql/include_all.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include "alexandria-core/library.h"
#include "alexandria-core/namespace.h"

namespace alex
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    Type::Type(Namespace& ns, std::string typeName, const bool instantiable) :
        nameSpace(&ns), name(std::move(typeName)), isInstance(instantiable)
    {
    }

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

    sql::Table& Type::getInstanceTable() const
    {
        requireCommitted();
        return *instanceTable;
    }

    const std::vector<sql::Table*>& Type::getPrimitiveArrayTables() const
    {
        requireCommitted();
        return primitiveArrayTables;
    }

    const std::vector<sql::Table*>& Type::getBlobArrayTables() const
    {
        requireCommitted();
        return blobArrayTables;
    }

    const std::vector<sql::Table*>& Type::getReferenceArrayTables() const
    {
        requireCommitted();
        return referenceArrayTables;
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

        if (!refType.isInstance)
            throw std::runtime_error(std::format(
              R"(Cannot add reference property to type {}::{}. Referenced type {}::{} is not an instantiable type.)",
              nameSpace->getName(),
              name,
              refType.nameSpace->getName(),
              refType.name));

        return createProperty(propName, DataType::Reference, &refType, false, false);
    }

    Property& Type::createReferenceArrayProperty(const std::string& propName, Type& refType)
    {
        requireNotCommitted();

        if (!refType.isInstance)
            throw std::runtime_error(std::format("Cannot add reference array property to type {}::{}. Referenced type "
                                                 "{}::{} is not an instantiable type.",
                                                 nameSpace->getName(),
                                                 name,
                                                 refType.nameSpace->getName(),
                                                 refType.name));

        return createProperty(propName, DataType::Reference, &refType, true, false);
    }

    Property& Type::createNestedTypeProperty(const std::string& propName, Type& nestedType)
    {
        requireNotCommitted();

        return createProperty(propName, DataType::Nested, &nestedType, false, false);
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

        auto& library = nameSpace->getLibrary();
        auto& db      = library.getDatabase();

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
            instanceTable = nullptr;
            primitiveArrayTables.clear();
            blobArrayTables.clear();
            referenceArrayTables.clear();
            throw;
        }
    }

    ////////////////////////////////////////////////////////////////
    // Private methods.
    ////////////////////////////////////////////////////////////////

    void Type::generate()
    {
        auto& library = nameSpace->getLibrary();
        auto& db      = library.getDatabase();

        // Create instance table.
        instanceTable = &db.createTable(nameSpace->getName() + "_" + name);
        instanceTable->createColumn("id", sql::Column::Type::Int).primaryKey(true);
        instanceTable->createColumn("uuid", sql::Column::Type::Text).unique();
        library.getGeneratedTablesInsert()(nullptr, id, sql::toText(instanceTable->getName()), sql::toText("instance"));

        // Add columns and array tables for all properties.
        for (const auto& prop : properties) prop->generate(*this, *instanceTable, "");

        // Commit instance table.
        instanceTable->commit();
    }

    Property& Type::createProperty(
      const std::string& propName, const DataType dataType, Type* refType, const bool isArray, const bool isBlob)
    {
        if (const auto it =
              std::ranges::find_if(properties, [&propName](const auto& prop) { return prop->getName() == propName; });
            it != properties.end())
            throw std::runtime_error(std::format(
              R"(Type "{}::{}" already has a property with name "{}".)", nameSpace->getName(), name, propName));

        if (const std::regex regex("^[a-z][a-z0-9_]*$"); !std::regex_match(propName, regex))
            throw std::runtime_error(std::format(
              R"(Cannot create property with name "{}". It does not match the regex "^[a-z][a-z0-9_]*$".)", propName));

        return *properties.emplace_back(
          std::make_unique<Property>(*this, -1, propName, dataType, refType, isArray, isBlob));
    }

    void Type::addProperty(PropertyPtr prop) { properties.emplace_back(std::move(prop)); }
}  // namespace alex
