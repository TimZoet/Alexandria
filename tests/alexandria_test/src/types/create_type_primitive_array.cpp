#include "alexandria_test/types/create_type_primitive_array.h"

void CreateTypePrimitiveArray::operator()()
{
    // Create type and property.
    alex::Type* type = nullptr;
    expectNoThrow([&] {
        type = &nameSpace->createType("type");
        type->createPrimitiveArrayProperty("p0", alex::DataType::Int32);
        type->createPrimitiveArrayProperty("p1", alex::DataType::Int64);
        type->createPrimitiveArrayProperty("p2", alex::DataType::Float);
        type->createPrimitiveArrayProperty("p3", alex::DataType::Double);
    });

    // Should not be able to add non-primitive properties like this.
    expectThrow([&] { type->createPrimitiveArrayProperty("p4", alex::DataType::Blob); });
    expectThrow([&] { type->createPrimitiveArrayProperty("p5", alex::DataType::Reference); });
    expectThrow([&] { type->createPrimitiveArrayProperty("p6", alex::DataType::String); });

    // Commit.
    expectNoThrow([&] { type->commit(); });

    // Check type tables.
    const std::vector<alex::NamespaceRow> namespaces = {{1, "main"}};
    const std::vector<alex::TypeRow>      types      = {{1, 1, "type", true}};
    const std::vector<alex::PropertyRow>  properties = {{1, 1, "p0", toString(alex::DataType::Int32), 0, true, false},
                                                        {2, 1, "p1", toString(alex::DataType::Int64), 0, true, false},
                                                        {3, 1, "p2", toString(alex::DataType::Float), 0, true, false},
                                                        {4, 1, "p3", toString(alex::DataType::Double), 0, true, false}};
    checkTypeTables(namespaces, types, properties);
}
