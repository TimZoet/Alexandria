#include "alexandria-core_test/types/create_type_blob.h"

void CreateTypeBlob::operator()()
{
    alex::TypeLayout layout;
    expectNoThrow([&] { layout.createBlobProperty("prop"); });
    compareEQ(alex::TypeLayout::Commit::Created, layout.commit(*nameSpace, "type").first);

    // Check type tables.
    const std::vector<alex::NamespaceRow> namespaces = {{1, "main"}};
    const std::vector<alex::TypeRow>      types      = {{1, 1, "type", true}};
    const std::vector<alex::PropertyRow> properties = {{1, 1, "prop", toString(alex::DataType::Blob), 0, false, false}};
    const std::vector<alex::TableRow>    tables     = {{1, 1, "main_type", "instance"}};
    checkTypeTables(namespaces, types, properties, tables);

    // TODO: For this and other create_type tests, also check generated tables.
}
