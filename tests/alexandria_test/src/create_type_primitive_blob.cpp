#include "alexandria_test/create_type_primitive_blob.h"

void CreateTypePrimitiveBlob::operator()()
{
    // Create type and property.
    alex::Type*     type;
    alex::Property *prop0, *prop1, *prop2;
    expectNoThrow([&type, this]() { type = &library->createType("type0"); });
    expectNoThrow(
      [&prop0, this]() { prop0 = &library->createPrimitiveArrayProperty("prop0", alex::DataType::Int32, true); });
    expectNoThrow(
      [&prop1, this]() { prop1 = &library->createPrimitiveArrayProperty("prop1", alex::DataType::Int64, true); });
    expectNoThrow(
      [&prop2, this]() { prop2 = &library->createPrimitiveArrayProperty("prop2", alex::DataType::Float, true); });
    expectThrow([this]() { library->createPrimitiveArrayProperty("prop3", alex::DataType::String, true); });
    expectNoThrow([&type, &prop0]() { type->addProperty(*prop0); });
    expectNoThrow([&type, &prop1]() { type->addProperty(*prop1); });
    expectNoThrow([&type, &prop2]() { type->addProperty(*prop2); });

    // Commit.
    expectNoThrow([this]() { library->commitTypes(); });

    // Check type tables.
    std::vector<utils::Type>     types      = {{0, "type0"}};
    std::vector<utils::Property> properties = {{0, "prop0", alex::toString(alex::DataType::Int32), 0, 0, 1, 1},
                                               {0, "prop1", alex::toString(alex::DataType::Int64), 0, 0, 1, 1},
                                               {0, "prop2", alex::toString(alex::DataType::Float), 0, 0, 1, 1}};
    std::vector<utils::Member>   members    = {
      {0, type->getId(), prop0->getId()}, {0, type->getId(), prop1->getId()}, {0, type->getId(), prop2->getId()}};
    checkTypeTables(std::move(types), std::move(properties), std::move(members));
}
