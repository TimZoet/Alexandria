#include "alexandria_test/types/create_type_reference.h"

void CreateTypeReference::operator()()
{
    // Create types and properties.
    // type0 -> prop0(type1&)
    // type1 -> prop1(type2&)
    //       \
    //        -> prop2(type3&)
    // type3 -> prop3
    alex::Type *    type0, *type1, *type2, *type3;
    alex::Property *prop0, *prop1, *prop2, *prop3;
    expectNoThrow([&type0, this]() { type0 = &library->createType("type0"); });
    expectNoThrow([&type1, this]() { type1 = &library->createType("type1"); });
    expectNoThrow([&type2, this]() { type2 = &library->createType("type2"); });
    expectNoThrow([&type3, this]() { type3 = &library->createType("type3"); });

    expectNoThrow([&type0, &type1, &prop0, this]() { prop0 = &type0->createReferenceProperty("prop0", *type1); });
    expectNoThrow([&type1, &type2, &type3, &prop1, &prop2, this]() {
        prop1 = &type1->createReferenceProperty("prop1", *type2);
        prop2 = &type1->createReferenceProperty("prop2", *type3);
    });
    expectNoThrow(
      [&type3, &prop3, this]() { prop3 = &type3->createPrimitiveProperty("prop3", alex::DataType::Float); });

    // Commit.
    expectNoThrow([this]() { library->commitTypes(); });

    // Check type tables.
    // When types and properties are committed, their dependencies are committed first; hence the order here.
    std::vector<utils::Type>     types      = {{0, "type0"}, {0, "type1"}, {0, "type2"}, {0, "type3"}};
    std::vector<utils::Property> properties = {
      {0, "prop1", alex::toString(alex::DataType::NestedType), type2->getId(), 1, 0, 0},
      {0, "prop3", alex::toString(alex::DataType::Float), 0, 0, 0, 0},
      {0, "prop2", alex::toString(alex::DataType::NestedType), type3->getId(), 1, 0, 0},
      {0, "prop0", alex::toString(alex::DataType::NestedType), type1->getId(), 1, 0, 0},
    };
    std::vector<utils::Member> members = {{0, type3->getId(), prop3->getId()},
                                          {0, type1->getId(), prop1->getId()},
                                          {0, type1->getId(), prop2->getId()},
                                          {0, type0->getId(), prop0->getId()}};
    checkTypeTables(std::move(types), std::move(properties), std::move(members));
}
