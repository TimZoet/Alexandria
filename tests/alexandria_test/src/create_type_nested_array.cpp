#include "alexandria_test/create_type_nested_array.h"

void CreateTypeNestedArray::operator()()
{
    // Create types and properties.
    // type0 -> prop1(type1[])
    // type1 -> prop2(type2[])
    //       \
    //        -> prop3(type3[])
    // type3 -> propA
    alex::Type *    type0, *type1, *type2, *type3;
    alex::Property *prop1, *prop2, *prop3, *propA;
    expectNoThrow([&type0, this]() { type0 = &library->createType("type0"); });
    expectNoThrow([&type1, this]() { type1 = &library->createType("type1"); });
    expectNoThrow([&type2, this]() { type2 = &library->createType("type2"); });
    expectNoThrow([&type3, this]() { type3 = &library->createType("type3"); });

    expectNoThrow([&prop1, &type1, this]() { prop1 = &library->createNestedArrayProperty("prop1", *type1); });
    expectNoThrow([&prop2, &type2, this]() { prop2 = &library->createNestedArrayProperty("prop2", *type2); });
    expectNoThrow([&prop3, &type3, this]() { prop3 = &library->createNestedArrayProperty("prop3", *type3); });
    expectNoThrow([&propA, this]() { propA = &library->createPrimitiveProperty("propA", alex::DataType::Float); });
    expectNoThrow([&type0, &prop1]() { type0->addProperty(*prop1); });
    expectNoThrow([&type1, &prop2]() { type1->addProperty(*prop2); });
    expectNoThrow([&type1, &prop3]() { type1->addProperty(*prop3); });
    expectNoThrow([&type3, &propA]() { type3->addProperty(*propA); });

    // Commit.
    expectNoThrow([this]() { library->commitTypes(); });

    // Check type tables.
    // When types and properties are committed, their dependencies are committed first; hence the order here.
    std::vector<utils::Type>     types      = {{0, "type0"}, {0, "type1"}, {0, "type2"}, {0, "type3"}};
    std::vector<utils::Property> properties = {
      {0, "prop2", alex::toString(alex::DataType::NestedType), type2->getId(), 1, 1, 0},
      {0, "propA", alex::toString(alex::DataType::Float), 0, 0, 0, 0},
      {0, "prop3", alex::toString(alex::DataType::NestedType), type3->getId(), 1, 1, 0},
      {0, "prop1", alex::toString(alex::DataType::NestedType), type1->getId(), 1, 1, 0},
    };
    std::vector<utils::Member> members = {{0, type3->getId(), propA->getId()},
                                          {0, type1->getId(), prop2->getId()},
                                          {0, type1->getId(), prop3->getId()},
                                          {0, type0->getId(), prop1->getId()}};
    checkTypeTables(std::move(types), std::move(properties), std::move(members));
}
