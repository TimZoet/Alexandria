#include "alexandria_test/types/create_type_reference_array.h"

void CreateTypeReferenceArray::operator()()
{
    // Create types and properties.
    // type0  -> prop0(type1[])
    // type1  -> prop1(type2[])
    //      \ -> prop2(type3[])
    // type3  -> prop3
    alex::Type *type0 = nullptr, *type1 = nullptr, *type2 = nullptr, *type3 = nullptr, *type4 = nullptr;
    expectNoThrow([&] { type0 = &nameSpace->createType("type0"); });
    expectNoThrow([&] { type1 = &nameSpace->createType("type1"); });
    expectNoThrow([&] { type2 = &nameSpace->createType("type2"); });
    expectNoThrow([&] { type3 = &nameSpace->createType("type3"); });
    expectNoThrow([&] { type4 = &nameSpace->createType("type4", false); });

    expectNoThrow([&] { type0->createReferenceArrayProperty("prop0", *type1); });
    expectNoThrow([&] {
        type1->createReferenceArrayProperty("prop1", *type2);
        type1->createReferenceArrayProperty("prop2", *type3);
    });
    expectThrow([&] { type1->createReferenceArrayProperty("prop3", *type4); });
    expectNoThrow([&] { type2->createPrimitiveProperty("prop3", alex::DataType::Int64); });
    expectNoThrow([&] { type3->createPrimitiveProperty("prop4", alex::DataType::Float); });

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
      {1, 1, "prop4", toString(alex::DataType::Float), 0, false, false},
      {2, 2, "prop3", toString(alex::DataType::Int64), 0, false, false},
      {3, 3, "prop1", toString(alex::DataType::Reference), 2, true, false},
      {4, 3, "prop2", toString(alex::DataType::Reference), 1, true, false},
      {5, 4, "prop0", toString(alex::DataType::Reference), 3, true, false}};
    checkTypeTables(namespaces, types, properties);
}
