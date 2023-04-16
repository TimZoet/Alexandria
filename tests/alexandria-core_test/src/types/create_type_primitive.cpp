#include "alexandria-core_test/types/create_type_primitive.h"

void CreateTypePrimitive::operator()()
{
    // Create type and property.
    alex::TypeLayout layout;
    expectNoThrow([&] {
        layout.createPrimitiveProperty("p0", alex::DataType::Int32);
        layout.createPrimitiveProperty("p1", alex::DataType::Int64);
        layout.createPrimitiveProperty("p2", alex::DataType::Float);
        layout.createPrimitiveProperty("p3", alex::DataType::Double);
    });

    // Should not be able to add non-primitive properties like this.
    expectThrow([&] { layout.createPrimitiveProperty("p4", alex::DataType::Blob); });
    expectThrow([&] { layout.createPrimitiveProperty("p5", alex::DataType::Reference); });
    expectThrow([&] { layout.createPrimitiveProperty("p6", alex::DataType::String); });

    // Commit.
    compareEQ(alex::TypeLayout::Commit::Created, layout.commit(*nameSpace, "type").first);

    // Check type tables.
    const std::vector<alex::NamespaceRow> namespaces = {{1, "main"}};
    const std::vector<alex::TypeRow>      types      = {{1, 1, "type", true}};
    const std::vector<alex::PropertyRow>  properties = {{1, 1, "p0", toString(alex::DataType::Int32), 0, false, false},
                                                        {2, 1, "p1", toString(alex::DataType::Int64), 0, false, false},
                                                        {3, 1, "p2", toString(alex::DataType::Float), 0, false, false},
                                                        {4, 1, "p3", toString(alex::DataType::Double), 0, false, false}};
    const std::vector<alex::TableRow>     tables     = {{1, 1, "main_type", "instance"}};
    checkTypeTables(namespaces, types, properties, tables);
}
