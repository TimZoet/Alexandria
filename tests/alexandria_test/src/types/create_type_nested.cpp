#include "alexandria_test/types/create_type_nested.h"

void CreateTypeNested::operator()()
{
    // Create types and properties.
    // type0 -> prop1(type1) -> prop2(type2)
    //       \
    //        -> prop3(type3)
    // type2 -> propA
    // type3 -> propB
    //       \
    //        -> propC
    alex::Type *type0 = nullptr, *type1 = nullptr, *type2 = nullptr, *type3 = nullptr;
    expectNoThrow([&] { type0 = &nameSpace->createType("type0"); });
    expectNoThrow([&] { type1 = &nameSpace->createType("type1"); });
    expectNoThrow([&] { type2 = &nameSpace->createType("type2"); });
    expectNoThrow([&] { type3 = &nameSpace->createType("type3"); });

    expectNoThrow([&] { type0->createNestedTypeProperty("prop1", *type1); });
    expectNoThrow([&] { type0->createNestedTypeProperty("prop3", *type3); });
    expectNoThrow([&] { type1->createNestedTypeProperty("prop2", *type2); });
    expectNoThrow([&] { type2->createPrimitiveProperty("propa", alex::DataType::Float); });
    expectNoThrow([&] { type3->createPrimitiveProperty("propb", alex::DataType::Double); });
    expectNoThrow([&] { type3->createPrimitiveProperty("propc", alex::DataType::Int32); });

    // Referenced types must be committed first.
    expectThrow([&] { type0->commit(); });
    expectThrow([&] { type1->commit(); });

    // Commit.
    expectNoThrow([&] { type3->commit(); });
    expectNoThrow([&] { type2->commit(); });
    expectNoThrow([&] { type1->commit(); });
    expectNoThrow([&] { type0->commit(); });

    // Check type tables.
    const std::vector<alex::NamespaceRow> namespaces = {{1, "main"}};
    const std::vector<alex::TypeRow>      types      = {
      {1, 1, "type3", true}, {2, 1, "type2", true}, {3, 1, "type1", true}, {4, 1, "type0", true}};
    const std::vector<alex::PropertyRow> properties = {
      {1, 1, "propb", toString(alex::DataType::Double), 0, false, false},
      {2, 1, "propc", toString(alex::DataType::Int32), 0, false, false},
      {3, 2, "propa", toString(alex::DataType::Float), 0, false, false},
      {4, 3, "prop2", toString(alex::DataType::Nested), 2, false, false},
      {5, 4, "prop1", toString(alex::DataType::Nested), 3, false, false},
      {6, 4, "prop3", toString(alex::DataType::Nested), 1, false, false}};
    checkTypeTables(namespaces, types, properties);
}
