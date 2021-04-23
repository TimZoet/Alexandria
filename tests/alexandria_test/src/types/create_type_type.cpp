#include "alexandria_test/types/create_type_type.h"

void CreateTypeType::operator()()
{
    // Create types and properties.
    // type0 -> prop1(type1) -> prop2(type2)
    //       \
    //        -> prop3(type3)
    // type2 -> propA
    // type3 -> propB
    // type3 -> propC
    alex::Type *    type0, *type1, *type2, *type3;
    alex::Property *prop1, *prop2, *prop3, *propA, *propB, *propC;
    expectNoThrow([&type0, this]() { type0 = &library->createType("type0"); });
    expectNoThrow([&type1, this]() { type1 = &library->createType("type1"); });
    expectNoThrow([&type2, this]() { type2 = &library->createType("type2"); });
    expectNoThrow([&type3, this]() { type3 = &library->createType("type3"); });

    expectNoThrow([&type0, &type1, &prop1]() { prop1 = &type0->createTypeProperty("prop1", *type1); });
    expectNoThrow([&type0, &type3, &prop3]() { prop3 = &type0->createTypeProperty("prop3", *type3); });
    expectNoThrow([&type1, &type2, &prop2]() { prop2 = &type1->createTypeProperty("prop2", *type2); });
    expectNoThrow([&type2, &propA]() { propA = &type2->createPrimitiveProperty("propA", alex::DataType::Float); });
    expectNoThrow([&type3, &propB]() { propB = &type3->createPrimitiveProperty("propB", alex::DataType::Double); });
    expectNoThrow([&type3, &propC]() { propC = &type3->createPrimitiveProperty("propC", alex::DataType::Int32); });

    // Commit.
    expectNoThrow([this]() { library->commitTypes(); });

    // Check type tables.
    // When types and properties are committed, their dependencies are committed first; hence the order here.
    std::vector<utils::Type>     types      = {{0, "type0"}, {0, "type1"}, {0, "type2"}, {0, "type3"}};
    std::vector<utils::Property> properties = {
      {0, "propA", alex::toString(alex::DataType::Float), 0, 0, 0, 0},
      {0, "prop2", alex::toString(alex::DataType::Type), type2->getId(), 0, 0, 0},
      {0, "prop1", alex::toString(alex::DataType::Type), type1->getId(), 0, 0, 0},
      {0, "propB", alex::toString(alex::DataType::Double), 0, 0, 0, 0},
      {0, "propC", alex::toString(alex::DataType::Int32), 0, 0, 0, 0},
      {0, "prop3", alex::toString(alex::DataType::Type), type3->getId(), 0, 0, 0}};
    std::vector<utils::Member> members = {{0, type2->getId(), propA->getId()},
                                          {0, type1->getId(), prop2->getId()},
                                          {0, type3->getId(), propB->getId()},
                                          {0, type3->getId(), propC->getId()},
                                          {0, type0->getId(), prop1->getId()},
                                          {0, type0->getId(), prop3->getId()}};
    checkTypeTables(std::move(types), std::move(properties), std::move(members));
}
