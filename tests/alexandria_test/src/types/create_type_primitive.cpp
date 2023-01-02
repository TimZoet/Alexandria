#include "alexandria_test/types/create_type_primitive.h"

void CreateTypePrimitive::operator()()
{
    // Create type and property.
    alex::Type* type = nullptr;
    expectNoThrow([&] {
        type = &nameSpace->createType("type");
        type->createPrimitiveProperty("p0", alex::DataType::Int32);
        type->createPrimitiveProperty("p1", alex::DataType::Int64);
        type->createPrimitiveProperty("p2", alex::DataType::Float);
        type->createPrimitiveProperty("p3", alex::DataType::Double);
    });

    // Should not be able to add non-primitive properties like this.
    expectThrow([&] { type->createPrimitiveProperty("p4", alex::DataType::Blob); });
    expectThrow([&] { type->createPrimitiveProperty("p5", alex::DataType::Reference); });
    expectThrow([&] { type->createPrimitiveProperty("p6", alex::DataType::String); });

    // Commit.
    expectNoThrow([&] { type->commit(); });

    // Check type tables.
    const std::vector<alex::NamespaceRow> namespaces = {{1, "main"}};
    const std::vector<alex::TypeRow>      types      = {{1, 1, "type", true}};
    const std::vector<alex::PropertyRow>  properties = {{1, 1, "p0", toString(alex::DataType::Int32), 0, false, false},
                                                        {2, 1, "p1", toString(alex::DataType::Int64), 0, false, false},
                                                        {3, 1, "p2", toString(alex::DataType::Float), 0, false, false},
                                                        {4, 1, "p3", toString(alex::DataType::Double), 0, false, false}};
    checkTypeTables(namespaces, types, properties);
}
