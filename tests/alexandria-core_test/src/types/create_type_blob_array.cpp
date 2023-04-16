#include "alexandria-core_test/types/create_type_blob_array.h"

void CreateTypeBlobArray::operator()()
{
    alex::TypeLayout layout;
    expectNoThrow([&] { layout.createBlobArrayProperty("prop"); });
    compareEQ(alex::TypeLayout::Commit::Created, layout.commit(*nameSpace, "type").first);

    // Check type tables.
    const std::vector<alex::NamespaceRow> namespaces = {{1, "main"}};
    const std::vector<alex::TypeRow>      types      = {{1, 1, "type", true}};
    const std::vector<alex::PropertyRow>  properties = {{1, 1, "prop", toString(alex::DataType::Blob), 0, true, false}};
    const std::vector<alex::TableRow>     tables     = {{1, 1, "main_type", "instance"},
                                                        {2, 1, "main_type_prop", "blob_array"}};
    checkTypeTables(namespaces, types, properties, tables);
}
