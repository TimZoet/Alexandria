#include "alexandria-core_test/types/create_type_primitive_array.h"

void CreateTypePrimitiveArray::operator()()
{
    alex::TypeLayout layout;
    expectNoThrow([&] {
        layout.createPrimitiveArrayProperty("p0", alex::DataType::Int32);
        layout.createPrimitiveArrayProperty("p1", alex::DataType::Int64);
        layout.createPrimitiveArrayProperty("p2", alex::DataType::Float);
        layout.createPrimitiveArrayProperty("p3", alex::DataType::Double);
    });

    // Should not be able to add non-primitive properties like this.
    expectThrow([&] { layout.createPrimitiveArrayProperty("p4", alex::DataType::Blob); });
    expectThrow([&] { layout.createPrimitiveArrayProperty("p5", alex::DataType::Reference); });
    expectThrow([&] { layout.createPrimitiveArrayProperty("p6", alex::DataType::String); });

    // Commit.
    compareEQ(alex::TypeLayout::Commit::Created, layout.commit(*nameSpace, "type").first);

    // Check type tables.
    const std::vector<alex::NamespaceRow> namespaces = {{1, "main"}};
    const std::vector<alex::TypeRow>      types      = {{1, 1, "type", true}};
    const std::vector<alex::PropertyRow>  properties = {{1, 1, "p0", toString(alex::DataType::Int32), 0, true, false},
                                                        {2, 1, "p1", toString(alex::DataType::Int64), 0, true, false},
                                                        {3, 1, "p2", toString(alex::DataType::Float), 0, true, false},
                                                        {4, 1, "p3", toString(alex::DataType::Double), 0, true, false}};
    const std::vector<alex::TableRow>     tables     = {{1, 1, "main_type", "instance"},
                                                        {2, 1, "main_type_p0", "primitive_array"},
                                                        {3, 1, "main_type_p1", "primitive_array"},
                                                        {4, 1, "main_type_p2", "primitive_array"},
                                                        {5, 1, "main_type_p3", "primitive_array"}};
    checkTypeTables(namespaces, types, properties, tables);
}
