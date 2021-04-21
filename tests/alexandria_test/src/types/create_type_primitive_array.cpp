#include "alexandria_test/types/create_type_primitive_array.h"

void CreateTypePrimitiveArray::operator()()
{
    // Create type and property.
    alex::Type*     type;
    alex::Property *prop0, *prop1, *prop2, *prop3;
    expectNoThrow([&type, &prop0, &prop1, &prop2, this]() {
        type  = &library->createType("type0");
        prop0 = &type->createPrimitiveArrayProperty("prop0", alex::DataType::Int32);
        prop1 = &type->createPrimitiveArrayProperty("prop1", alex::DataType::Int64);
        prop2 = &type->createPrimitiveArrayProperty("prop2", alex::DataType::Float);
    });

    // Commit.
    expectNoThrow([this]() { library->commitTypes(); });

    // Check type tables.
    std::vector<utils::Type>     types      = {{0, "type0"}};
    std::vector<utils::Property> properties = {{0, "prop0", alex::toString(alex::DataType::Int32), 0, 1, 1, 0},
                                               {0, "prop1", alex::toString(alex::DataType::Int64), 0, 1, 1, 0},
                                               {0, "prop2", alex::toString(alex::DataType::Float), 0, 1, 1, 0}};
    std::vector<utils::Member>   members    = {
      {0, type->getId(), prop0->getId()}, {0, type->getId(), prop1->getId()}, {0, type->getId(), prop2->getId()}};
    checkTypeTables(std::move(types), std::move(properties), std::move(members));
}
