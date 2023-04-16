#include "alexandria-core_test/types/create_type_primitive_blob.h"

void CreateTypePrimitiveBlob::operator()()
{
    alex::TypeLayout layout;
    expectNoThrow([&] {
        layout.createPrimitiveBlobProperty("p0", alex::DataType::Int32);
        layout.createPrimitiveBlobProperty("p1", alex::DataType::Int64);
        layout.createPrimitiveBlobProperty("p2", alex::DataType::Float);
        layout.createPrimitiveBlobProperty("p3", alex::DataType::Double);
    });

    // Should not be able to add non-primitive properties like this.
    expectThrow([&] { layout.createPrimitiveBlobProperty("p4", alex::DataType::Blob); });
    expectThrow([&] { layout.createPrimitiveBlobProperty("p5", alex::DataType::Reference); });
    expectThrow([&] { layout.createPrimitiveBlobProperty("p6", alex::DataType::String); });

    // Commit.
    compareEQ(alex::TypeLayout::Commit::Created, layout.commit(*nameSpace, "type").first);

    // Check type tables.
    const std::vector<alex::NamespaceRow> namespaces = {{1, "main"}};
    const std::vector<alex::TypeRow>      types      = {{1, 1, "type", true}};
    const std::vector<alex::PropertyRow>  properties = {{1, 1, "p0", toString(alex::DataType::Int32), 0, false, true},
                                                        {2, 1, "p1", toString(alex::DataType::Int64), 0, false, true},
                                                        {3, 1, "p2", toString(alex::DataType::Float), 0, false, true},
                                                        {4, 1, "p3", toString(alex::DataType::Double), 0, false, true}};
    const std::vector<alex::TableRow>     tables     = {{1, 1, "main_type", "instance"}};
    checkTypeTables(namespaces, types, properties, tables);
}
