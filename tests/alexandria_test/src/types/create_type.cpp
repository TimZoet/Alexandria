#include "alexandria_test/types/create_type.h"

void CreateType::operator()()
{
    // Create several types.
    alex::Type *type0, *type1, *type2;
    expectNoThrow([&type0, this]() { type0 = &library->createType("type0"); });
    expectNoThrow([&type1, this]() { type1 = &library->createType("type1"); });
    expectNoThrow([&type2, this]() { type2 = &library->createType("type2"); });

    // Recreating types with same name should throw.
    expectThrow([this]() { library->createType("type0"); });
    expectThrow([this]() { library->createType("type1"); });
    expectThrow([this]() { library->createType("type2"); });
    // TODO: Test creation of types with other forbidden names.
    // Do the same for properties.

    // Check name and committed.
    compareEQ(type0->getName(), "type0");
    compareEQ(type1->getName(), "type1");
    compareEQ(type2->getName(), "type2");
    compareFalse(type0->isCommitted());
    compareFalse(type1->isCommitted());
    compareFalse(type2->isCommitted());

    // Commit.
    expectNoThrow([&type0, this]() { library->commitType(*type0); });
    expectNoThrow([&type1, this]() { library->commitType(*type1); });
    expectNoThrow([&type2, this]() { library->commitType(*type2); });

    // Check committed.
    compareTrue(type0->isCommitted());
    compareTrue(type1->isCommitted());
    compareTrue(type2->isCommitted());
}
