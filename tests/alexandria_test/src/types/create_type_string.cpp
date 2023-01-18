#include "alexandria_test/types/create_type_string.h"

void CreateTypeString::operator()()
{
    // Create type and property.
    alex::Type* type = nullptr;
    expectNoThrow([&] {
        type = &nameSpace->createType("type");
        type->createStringProperty("prop");
    });

    // Commit.
    expectNoThrow([&] { type->commit(); });

    // Check type tables.
    const std::vector<alex::NamespaceRow> namespaces = {{1, "main"}};
    const std::vector<alex::TypeRow>      types      = {{1, 1, "type", true}};
    const std::vector<alex::PropertyRow>  properties = {
      {1, 1, "prop", toString(alex::DataType::String), 0, false, false}};
    const std::vector<alex::TableRow> tables = {{1, 1, "main_type", "instance"}};
    checkTypeTables(namespaces, types, properties, tables);
}
