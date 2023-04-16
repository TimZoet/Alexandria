#include "alexandria-core_test/types/create_type_reference.h"

void CreateTypeReference::operator()()
{
    // TODO: Test creation of reference cycles, references to self, etc. Same for ReferenceArray.

    // Create types and properties.
    // type0  -> prop0(type1)
    // type1  -> prop1(type2)
    //      \ -> prop2(type3)
    // type2  -> prop3(int64)
    // type3  -> prop4(float)
    // type4  -> prop5(double)

    alex::TypeLayout layout0;
    alex::TypeLayout layout1;
    alex::TypeLayout layout2;
    alex::TypeLayout layout3;
    alex::TypeLayout layout4;

    expectNoThrow([&] { layout4.createPrimitiveProperty("prop5", alex::DataType::Double); });
    auto [commit4, type4] = layout4.commit(*nameSpace, "type4", alex::TypeLayout::Instantiable::False);
    compareEQ(alex::TypeLayout::Commit::Created, commit4);

    expectNoThrow([&] { layout3.createPrimitiveProperty("prop4", alex::DataType::Float); });
    auto [commit3, type3] = layout3.commit(*nameSpace, "type3");
    compareEQ(alex::TypeLayout::Commit::Created, commit3);

    expectNoThrow([&] { layout2.createPrimitiveProperty("prop3", alex::DataType::Int64); });
    auto [commit2, type2] = layout2.commit(*nameSpace, "type2");
    compareEQ(alex::TypeLayout::Commit::Created, commit2);

    expectThrow([&] { layout1.createReferenceProperty("prop3", *type4); });
    expectNoThrow([&] {
        layout1.createReferenceProperty("prop1", *type2);
        layout1.createReferenceProperty("prop2", *type3);
    });
    auto [commit1, type1] = layout1.commit(*nameSpace, "type1");
    compareEQ(alex::TypeLayout::Commit::Created, commit1);

    expectNoThrow([&] { layout0.createReferenceProperty("prop0", *type1); });
    auto [commit0, type0] = layout0.commit(*nameSpace, "type0");
    compareEQ(alex::TypeLayout::Commit::Created, commit0);

    // Check type tables.
    const std::vector<alex::NamespaceRow> namespaces = {{1, "main"}};
    const std::vector<alex::TypeRow>      types      = {{1, 1, "type4", false},
                                                        {2, 1, "type3", true},
                                                        {3, 1, "type2", true},
                                                        {4, 1, "type1", true},
                                                        {5, 1, "type0", true}};
    const std::vector<alex::PropertyRow>  properties = {
      {1, 1, "prop5", toString(alex::DataType::Double), 0, false, false},
      {2, 2, "prop4", toString(alex::DataType::Float), 0, false, false},
      {3, 3, "prop3", toString(alex::DataType::Int64), 0, false, false},
      {4, 4, "prop1", toString(alex::DataType::Reference), 3, false, false},
      {5, 4, "prop2", toString(alex::DataType::Reference), 2, false, false},
      {6, 5, "prop0", toString(alex::DataType::Reference), 4, false, false}};
    const std::vector<alex::TableRow> tables = {{1, 2, "main_type3", "instance"},
                                                {2, 3, "main_type2", "instance"},
                                                {3, 4, "main_type1", "instance"},
                                                {4, 5, "main_type0", "instance"}};
    checkTypeTables(namespaces, types, properties, tables);
}
