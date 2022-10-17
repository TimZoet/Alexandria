#include "alexandria/type.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <algorithm>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "cppql-typed/queries/insert.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <stdexcept>

namespace alex
{
    Type::Type(std::string typeName) : id(-1), name(std::move(typeName)) {}

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    bool Type::isCommitted() const noexcept { return id != -1; }

    sql::row_id Type::getId() const noexcept { return id; }

    const std::string& Type::getName() const noexcept { return name; }

    const PropertyList& Type::getProperties() const noexcept { return properties; }

    std::vector<Property*> Type::getProperties(const std::function<bool(const Property&)>& predicate) const
    {
        std::vector<Property*>         props;
        std::function<void(Property&)> filter;
        filter = [&props, &predicate, &filter](Property& prop) {
            // Test predicate.
            if (predicate(prop)) props.push_back(&prop);

            // Recurse on member types that are expanded.
            if (prop.getDataType() == DataType::Type && !prop.isReference())
            {
                for (const auto& p : prop.getReferenceType()->getProperties()) filter(*p);
            }
        };

        for (const auto& prop : properties) filter(*prop);

        return props;
    }

    std::vector<Property*> Type::getPropertyParents(const Property& prop) const
    {
        std::vector<Property*>         parents;
        std::function<bool(Property&)> traverse;
        traverse = [&parents, &prop, &traverse](Property& p) {
            // Check if current property is property we are looking for.
            if (&prop == &p) return true;

            // Push.
            parents.push_back(&p);

            // Recurse on nested types that are expanded.
            if (p.getDataType() == DataType::Type && !p.isReference())
            {
                for (const auto& p_ : p.getReferenceType()->getProperties())
                {
                    if (traverse(*p_)) return true;
                }
            }

            // Pop.
            parents.pop_back();

            // Did not find property here.
            return false;
        };

        for (const auto& p : properties)
        {
            if (traverse(*p)) return parents;
        }

        throw std::runtime_error("Property is not a member of this type");
    }

    std::string Type::resolveReferenceTableName(const std::vector<Property*>& parents, const Property& prop) const
    {
        auto tableName = getName() + "_";
        for (const auto& p : parents) tableName += p->getName() + "_";
        tableName += prop.getName();
        return tableName;
    }

    const std::vector<Property*>& Type::getReferences() const noexcept { return references; }

    ////////////////////////////////////////////////////////////////
    // Properties.
    ////////////////////////////////////////////////////////////////

    Property& Type::createPrimitiveProperty(const std::string& propName, const DataType dataType)
    {
        if (!isPrimitiveDataType(dataType)) throw std::runtime_error("dataType is not a primitive type");

        return createProperty(propName, dataType, nullptr, false, false, false);
    }

    Property& Type::createStringProperty(const std::string& propName)
    {
        return createProperty(propName, DataType::String, nullptr, false, false, false);
    }

    Property& Type::createBlobProperty(const std::string& propName)
    {
        return createProperty(propName, DataType::Blob, nullptr, false, false, true);
    }

    Property& Type::createReferenceProperty(const std::string& propName, Type& refType)
    {
        return createProperty(propName, DataType::Type, &refType, true, false, false);
    }

    Property& Type::createTypeProperty(const std::string& propName, Type& memberType)
    {
        return createProperty(propName, DataType::Type, &memberType, false, false, false);
    }

    Property& Type::createPrimitiveArrayProperty(const std::string& propName, const DataType dataType)
    {
        if (!isPrimitiveDataType(dataType)) throw std::runtime_error("dataType is not a primitive type");

        return createProperty(propName, dataType, nullptr, true, true, false);
    }

    Property& Type::createStringArrayProperty(const std::string& propName)
    {
        return createProperty(propName, DataType::String, nullptr, true, true, false);
    }

    Property& Type::createReferenceArrayProperty(const std::string& propName, Type& refType)
    {
        return createProperty(propName, DataType::Type, &refType, true, true, false);
    }

    Property& Type::createBlobArrayProperty(const std::string& propName)
    {
        return createProperty(propName, DataType::Blob, nullptr, true, true, true);
    }

    Property& Type::createPrimitiveBlobProperty(const std::string& propName, const DataType dataType)
    {
        if (!isPrimitiveDataType(dataType)) throw std::runtime_error("dataType is not a primitive type");

        return createProperty(propName, dataType, nullptr, false, true, true);
    }

    ////////////////////////////////////////////////////////////////
    // Private methods.
    ////////////////////////////////////////////////////////////////

    Property& Type::createProperty(const std::string& propName,
                                   const DataType     dataType,
                                   Type*              refType,
                                   const bool         isReference,
                                   const bool         isArray,
                                   const bool         isBlob)
    {
        //if (properties.contains(propName)) throw std::runtime_error("A property with this name already exists");
        // Also check for other forbidden names:
        // Type names such as float, double, int32 etc.
        // ...

        // Create property.
        if (refType)
        {
            auto prop = std::make_unique<Property>(propName, *refType, isReference, isArray, isBlob);
            return *properties.emplace_back(std::move(prop));
        }

        auto prop = std::make_unique<Property>(propName, dataType, isReference, isArray, isBlob);
        return *properties.emplace_back(std::move(prop));
    }

    void Type::addProperty(PropertyPtr prop) { properties.emplace_back(std::move(prop)); }

    void Type::addReference(Property& prop) { references.push_back(&prop); }

    void Type::commit(sql::Database& db, TypeTable& typeTable, PropertyTable& propsTable, MemberTable& memTable)
    {
        if (isCommitted()) throw std::runtime_error("Type was already committed");

        // Add type to table.
        typeTable.insert()(nullptr, sql::StaticText{.data = getName().data(), .size = getName().size()});

        // Set ID.
        id = db.getLastInsertRowId();

        // Commit properties.
        for (const auto& prop : getProperties()) prop->commit(db, typeTable, propsTable, memTable);

        // Add links between type and all its properties to members table.
        auto insert2 = memTable.insert();
        for (const auto& prop : getProperties()) insert2(nullptr, getId(), prop->getId());

        // Create instance table.
        auto& instanceTable = db.createTable(getName());

        // Create ID column.
        instanceTable.createColumn("id", sql::Column::Type::Int)
          .setPrimaryKey(true)
          .setAutoIncrement(true)
          .setNotNull(true);

        // Add columns and reference tables for all properties.
        for (const auto& prop : getProperties()) prop->generateTables(db, instanceTable, "");

        // Commit instance table.
        instanceTable.commit();

        // Resolve references.
        resolve(db, instanceTable, "");
    }

    void Type::generateTables(sql::Database& db, sql::Table& table, const std::string& prefix) const
    {
        for (const auto& prop : getProperties()) prop->generateTables(db, table, prefix);
    }

    void Type::resolve(sql::Database& db, sql::Table& table, const std::string& prefix) const
    {
        if (!isCommitted()) throw std::runtime_error("Type was not yet committed");

        for (const auto& prop : getProperties()) prop->resolve(db, table, prefix);
    }
}  // namespace alex