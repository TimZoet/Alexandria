#include "alexandria-core/type_layout.h"

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

    TypeLayout::TypeLayout(const TypeLayout& other) { *this = other; }

    TypeLayout& TypeLayout::operator=(const TypeLayout& other)
    {
        for (const auto& prop : other.properties) properties.emplace_back(std::make_unique<PropertyLayout>(*prop));
        return *this;
    }

    bool TypeLayout::operator==(const TypeLayout& rhs) const noexcept
    {
        if (properties.size() != rhs.properties.size()) return false;
        for (size_t i = 0; i < properties.size(); i++)
        {
            if (*properties[i] != *rhs.properties[i]) return false;
        }

        return true;
    }

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    const std::vector<PropertyLayoutPtr>& TypeLayout::getProperties() const noexcept { return properties; }

    ////////////////////////////////////////////////////////////////
    // Properties.
    ////////////////////////////////////////////////////////////////

    PropertyLayout& TypeLayout::createPrimitiveProperty(const std::string& propName, const DataType dataType)
    {
        if (!isPrimitiveDataType(dataType)) throw std::runtime_error("dataType is not a primitive type");

        return createProperty(propName, dataType, nullptr, false, false);
    }

    PropertyLayout& TypeLayout::createPrimitiveArrayProperty(const std::string& propName, const DataType dataType)
    {
        if (!isPrimitiveDataType(dataType)) throw std::runtime_error("dataType is not a primitive type");

        return createProperty(propName, dataType, nullptr, true, false);
    }

    PropertyLayout& TypeLayout::createPrimitiveBlobProperty(const std::string& propName, const DataType dataType)
    {
        if (!isPrimitiveDataType(dataType)) throw std::runtime_error("dataType is not a primitive type");

        return createProperty(propName, dataType, nullptr, false, true);
    }

    PropertyLayout& TypeLayout::createStringProperty(const std::string& propName)
    {
        return createProperty(propName, DataType::String, nullptr, false, false);
    }

    PropertyLayout& TypeLayout::createStringArrayProperty(const std::string& propName)
    {
        return createProperty(propName, DataType::String, nullptr, true, false);
    }

    PropertyLayout& TypeLayout::createBlobProperty(const std::string& propName)
    {
        return createProperty(propName, DataType::Blob, nullptr, false, false);
    }

    PropertyLayout& TypeLayout::createBlobArrayProperty(const std::string& propName)
    {
        return createProperty(propName, DataType::Blob, nullptr, true, false);
    }

    PropertyLayout& TypeLayout::createReferenceProperty(const std::string& propName, Type& refType)
    {
        if (refType.isInstantiable() == Instantiable::False)
            throw std::runtime_error(std::format(
              R"(Cannot add reference property to layout. Referenced type {}::{} is not an instantiable type.)",
              refType.getNamespace().getName(),
              refType.getName()));

        return createProperty(propName, DataType::Reference, &refType, false, false);
    }

    PropertyLayout& TypeLayout::createReferenceArrayProperty(const std::string& propName, Type& refType)
    {
        if (refType.isInstantiable() == Instantiable::False)
            throw std::runtime_error(std::format("Cannot add reference array property to layout. Referenced type "
                                                 "{}::{} is not an instantiable type.",
                                                 refType.getNamespace().getName(),
                                                 refType.getName()));

        return createProperty(propName, DataType::Reference, &refType, true, false);
    }

    PropertyLayout& TypeLayout::createNestedTypeProperty(const std::string& propName, Type& nestedType)
    {
        return createProperty(propName, DataType::Nested, &nestedType, false, false);
    }

    ////////////////////////////////////////////////////////////////
    // Commit.
    ////////////////////////////////////////////////////////////////

    std::pair<TypeLayout::Commit, Type*>
      TypeLayout::commit(Namespace& nameSpace, std::string name, Instantiable instantiable) const
    {
        if (name.empty())
            throw std::runtime_error(
              std::format("Type {}::{} cannot be committed. It has no name.", nameSpace.getName(), name));
        if (properties.empty())
            throw std::runtime_error(
              std::format("Type {}::{} cannot be committed. It has no properties.", nameSpace.getName(), name));

        // Check if namespace contains identical type.
        if (Type* existingType = nullptr; nameSpace.getType(name, &existingType))
        {
            if (*existingType->typeLayout == *this && existingType->instantiable == instantiable)
                return {Commit::Existed, existingType};

            throw std::runtime_error(std::format(
              "Type {}::{} cannot be committed. Another type with the same name but a different layout already exists.",
              nameSpace.getName(),
              name));
        }

        try
        {
            auto&                    library = nameSpace.getLibrary();
            auto&                    db      = library.getDatabase();
            std::vector<sql::row_id> propertyIds;
            sql::Table*              instanceTable = nullptr;
            std::vector<sql::Table*> primitiveArrayTables;
            std::vector<sql::Table*> blobArrayTables;
            std::vector<sql::Table*> referenceArrayTables;

            auto transaction = db.beginTransaction(sql::Transaction::Type::Deferred);

            // Add type to table and retrieve ID.
            library.getTypeTableInsert()(
              nullptr, nameSpace.getId(), sql::toStaticText(name), instantiable == Instantiable::True ? 1 : 0);
            sql::row_id typeId = db.getLastInsertRowId();

            // Commit properties.
            for (const auto& prop : properties) propertyIds.push_back(prop->commit(nameSpace, typeId));

            // Generate tables.
            if (instantiable == Instantiable::True)
            {
                // Create instance table.
                instanceTable = &db.createTable(nameSpace.getName() + "_" + name);
                instanceTable->createColumn("id", sql::Column::Type::Int).primaryKey(true);
                instanceTable->createColumn("uuid", sql::Column::Type::Text).unique();
                library.getGeneratedTablesInsert()(
                  nullptr, typeId, sql::toText(instanceTable->getName()), sql::toText("instance"));

                // Add columns and array tables for all properties.
                for (const auto& prop : properties)
                    prop->generate(
                      library, typeId, *instanceTable, primitiveArrayTables, blobArrayTables, referenceArrayTables, "");

                // Commit instance table.
                instanceTable->commit();
            }

            auto& type                  = nameSpace.createType(name);
            type.id                     = typeId;
            type.propertyIds            = std::move(propertyIds);
            type.name                   = name;
            type.instantiable           = instantiable;
            type.typeLayout             = std::make_unique<TypeLayout>(*this);
            type.nameSpace              = &nameSpace;
            type.tables.instance        = instanceTable;
            type.tables.primitiveArrays = std::move(primitiveArrayTables);
            type.tables.blobArrays      = std::move(blobArrayTables);
            type.tables.referenceArrays = std::move(referenceArrayTables);

            transaction.commit();

            return {Commit::Created, &type};
        }
        catch (const std::exception&)
        {
            throw;
        }
    }

    ////////////////////////////////////////////////////////////////
    // Private methods.
    ////////////////////////////////////////////////////////////////

    PropertyLayout& TypeLayout::createProperty(
      const std::string& propName, const DataType dataType, Type* refType, const bool isArray, const bool isBlob)
    {
        if (const auto it =
              std::ranges::find_if(properties, [&propName](const auto& prop) { return prop->getName() == propName; });
            it != properties.end())
            throw std::runtime_error(std::format(R"(TypeLayout already has a property with name "{}".)", propName));

        if (const std::regex regex("^[a-z][a-z0-9_]*$"); !std::regex_match(propName, regex))
            throw std::runtime_error(std::format(
              R"(Cannot create property with name "{}". It does not match the regex "^[a-z][a-z0-9_]*$".)", propName));

        return *properties.emplace_back(
          std::make_unique<PropertyLayout>(*this, propName, dataType, refType, isArray, isBlob));
    }

    void TypeLayout::addProperty(PropertyLayoutPtr prop) { properties.emplace_back(std::move(prop)); }

}  // namespace alex
