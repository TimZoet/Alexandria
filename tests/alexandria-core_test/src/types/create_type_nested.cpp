#include "alexandria-core_test/types/create_type_nested.h"

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
    alex::TypeLayout layout0;
    alex::TypeLayout layout1;
    alex::TypeLayout layout2;
    alex::TypeLayout layout3;

    expectNoThrow([&] { layout3.createPrimitiveProperty("propb", alex::DataType::Double); });
    expectNoThrow([&] { layout3.createPrimitiveProperty("propc", alex::DataType::Int32); });
    auto [commit3, type3] = layout3.commit(*nameSpace, "type3");
    compareEQ(alex::TypeLayout::Commit::Created, commit3);

    expectNoThrow([&] { layout2.createPrimitiveProperty("propa", alex::DataType::Float); });
    auto [commit2, type2] = layout2.commit(*nameSpace, "type2");
    compareEQ(alex::TypeLayout::Commit::Created, commit2);

    expectNoThrow([&] { layout1.createNestedTypeProperty("prop2", *type2); });
    auto [commit1, type1] = layout1.commit(*nameSpace, "type1");
    compareEQ(alex::TypeLayout::Commit::Created, commit1);

    expectNoThrow([&] { layout0.createNestedTypeProperty("prop1", *type1); });
    expectNoThrow([&] { layout0.createNestedTypeProperty("prop3", *type3); });
    auto [commit0, type0] = layout0.commit(*nameSpace, "type0");
    compareEQ(alex::TypeLayout::Commit::Created, commit0);

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
    const std::vector<alex::TableRow> tables = {{1, 1, "main_type3", "instance"},
                                                {2, 2, "main_type2", "instance"},
                                                {3, 3, "main_type1", "instance"},
                                                {4, 4, "main_type0", "instance"}};
    checkTypeTables(namespaces, types, properties, tables);
}
