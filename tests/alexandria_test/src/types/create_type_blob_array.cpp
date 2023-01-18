#include "alexandria_test/types/create_type_blob_array.h"

void CreateTypeBlobArray::operator()()
{
    // Create type and property.
    alex::Type* type = nullptr;
    expectNoThrow([&] {
        type = &nameSpace->createType("type");
        type->createBlobArrayProperty("prop");
    });

    // Commit.
    expectNoThrow([&] { type->commit(); });

    // Check type tables.
    const std::vector<alex::NamespaceRow> namespaces = {{1, "main"}};
    const std::vector<alex::TypeRow>      types      = {{1, 1, "type", true}};
    const std::vector<alex::PropertyRow>  properties = {{1, 1, "prop", toString(alex::DataType::Blob), 0, true, false}};
    const std::vector<alex::TableRow>     tables     = {{1, 1, "main_type", "instance"},
                                                        {2, 1, "main_type_prop", "blob_array"}};
    checkTypeTables(namespaces, types, properties, tables);
}
