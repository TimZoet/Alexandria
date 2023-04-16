#include "alexandria-core/property_layout.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "alexandria-core/library.h"
#include "alexandria-core/namespace.h"
#include "alexandria-core/type.h"
#include "alexandria-core/type_layout.h"

namespace alex
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    PropertyLayout::PropertyLayout(TypeLayout&    layout,
                                   std::string    propName,
                                   const DataType type,
                                   Type*          refType,
                                   const bool     isArray,
                                   const bool     isBlob) :
        typeLayout(&layout),
        name(std::move(propName)),
        dataType(type),
        referenceType(refType),
        array(isArray),
        blob(isBlob)
    {
    }

    bool PropertyLayout::operator==(const PropertyLayout& rhs) const noexcept
    {
        return name == rhs.name && dataType == rhs.dataType && referenceType == rhs.referenceType &&
               array == rhs.array && blob == rhs.blob;
    }

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    const std::string& PropertyLayout::getName() const noexcept { return name; }

    DataType PropertyLayout::getDataType() const noexcept { return dataType; }

    Type* PropertyLayout::getReferenceType() noexcept { return referenceType; }

    const Type* PropertyLayout::getReferenceType() const noexcept { return referenceType; }

    bool PropertyLayout::isArray() const noexcept { return array; }

    bool PropertyLayout::isBlob() const noexcept { return blob; }

    sql::row_id PropertyLayout::commit(Namespace& nameSpace, sql::row_id typeId) const
    {
        const auto& library       = nameSpace.getLibrary();
        const auto& db            = library.getDatabase();
        const auto& propertyTable = library.getPropertyTable();

        // Add property to table.
        auto       insert     = propertyTable.insert().compile();
        const auto typeString = toString(dataType);
        insert(nullptr,
               typeId,
               sql::toStaticText(name),
               sql::toStaticText(typeString),
               referenceType ? std::make_optional(referenceType->getId()) : std::optional<sql::row_id>(),
               isArray() ? 1 : 0,
               isBlob() ? 1 : 0);

        // Set ID.
        return db.getLastInsertRowId();
    }

    void PropertyLayout::generate(Library&                  library,
                                  sql::row_id               currentType,
                                  sql::Table&               instanceTable,
                                  std::vector<sql::Table*>& primitiveArrayTables,
                                  std::vector<sql::Table*>& blobArrayTables,
                                  std::vector<sql::Table*>& referenceArrayTables,
                                  const std::string&        prefix) const
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

                if (dataType == DataType::Blob)
                {
                    blobArrayTables.emplace_back(&arrayTable);
                    library.getGeneratedTablesInsert()(
                      nullptr, currentType, sql::toText(arrayTable.getName()), sql::toText("blob_array"));
                }
                else
                {
                    primitiveArrayTables.emplace_back(&arrayTable);
                    library.getGeneratedTablesInsert()(
                      nullptr, currentType, sql::toText(arrayTable.getName()), sql::toText("primitive_array"));
                }
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
            for (const auto& prop : referenceType->getLayout().getProperties())
                prop->generate(library,
                               currentType,
                               instanceTable,
                               primitiveArrayTables,
                               blobArrayTables,
                               referenceArrayTables,
                               prefix2);
        }
        else if (dataType == DataType::Reference)
        {
            auto& refColumn = db.getTable(referenceType->getInstanceTable().getName()).getColumn("uuid");

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

                referenceArrayTables.emplace_back(&arrayTable);
                library.getGeneratedTablesInsert()(
                  nullptr, currentType, sql::toText(arrayTable.getName()), sql::toText("reference_array"));
            }
            else
            {
                // Add a column with a foreign key to the instance table of referenceType.
                instanceTable.createColumn(prefix + name, sql::Column::Type::Text)
                  .foreignKey(refColumn, sql::ForeignKeyAction::SetNull)
                  .check("");
            }
        }
    }
}  // namespace alex
