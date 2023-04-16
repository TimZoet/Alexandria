#include "alexandria-core_test/types/create_type.h"

void CreateType::operator()()
{
    alex::TypeLayout layout;

    // Committing empty layout should throw.
    expectThrow([&] { layout.commit(*nameSpace, "type"); });

    // Creating property with invalid name should throw.
    expectThrow([&] { layout.createPrimitiveProperty("5", alex::DataType::Float); });

    // Creating property with same name should throw.
    expectNoThrow([&] { layout.createPrimitiveProperty("prop", alex::DataType::Float); });
    expectThrow([&] { layout.createBlobProperty("prop"); });

    // Commit should create type.
    auto [commit0, type0] = layout.commit(*nameSpace, "type");
    compareEQ(alex::TypeLayout::Commit::Created, commit0);
    compareNE(nullptr, type0);

    // Another commit should return existing type.
    auto [commit1, type1] = layout.commit(*nameSpace, "type");
    compareEQ(alex::TypeLayout::Commit::Existed, commit1);
    compareEQ(type0, type1);

    // Committing a layout with a known name but different properties should throw.
    layout.createPrimitiveProperty("prop2", alex::DataType::Int32);
    expectThrow([&] { layout.commit(*nameSpace, "type"); });

    // Creating some types with invalid names.
    expectThrow([&] {
        alex::TypeLayout l;
        l.createPrimitiveProperty("prop", alex::DataType::Float);
        l.commit(*nameSpace, "0ad");
    });
    expectThrow([&] {
        alex::TypeLayout l;
        l.createPrimitiveProperty("prop", alex::DataType::Float);
        l.commit(*nameSpace, "^%%*)");
    });
    expectThrow([&] {
        alex::TypeLayout l;
        l.createPrimitiveProperty("prop", alex::DataType::Float);
        l.commit(*nameSpace, "ABD");
    });
}
