#include "alexandria_test/types/create_type.h"

void CreateType::operator()()
{
    // Create several types.
    alex::Type *type0 = nullptr, *type1 = nullptr, *type2 = nullptr;
    expectNoThrow([&] { type0 = &nameSpace->createType("type0"); });
    expectNoThrow([&] { type1 = &nameSpace->createType("type1"); });
    expectNoThrow([&] { type2 = &nameSpace->createType("type2_", false); });
    expectNoThrow([&] { nameSpace->createType("type3_aaaa034"); });

    // Recreating types with same name should throw.
    expectThrow([&] { nameSpace->createType("type0"); });
    expectThrow([&] { nameSpace->createType("type1"); });
    expectThrow([&] { nameSpace->createType("type2_"); });

    // Creating some types with invalid names.
    expectThrow([&] { nameSpace->createType("_abc"); });
    expectThrow([&] { nameSpace->createType("0ad"); });
    expectThrow([&] { nameSpace->createType("^%%*)"); });
    expectThrow([&] { nameSpace->createType("ABD"); });

    // Check name and committed.
    compareEQ(type0->getName(), "type0");
    compareEQ(type1->getName(), "type1");
    compareEQ(type2->getName(), "type2_");
    compareFalse(type0->isCommitted());
    compareFalse(type1->isCommitted());
    compareFalse(type2->isCommitted());

    // Commit without props should throw.
    expectThrow([&] { type0->commit(); });
    expectThrow([&] { type1->commit(); });
    expectThrow([&] { type2->commit(); });

    expectNoThrow([&] { type0->createPrimitiveProperty("a", alex::DataType::Float); });
    expectNoThrow([&] { type1->createPrimitiveProperty("b", alex::DataType::Float); });
    expectNoThrow([&] { type2->createPrimitiveProperty("c", alex::DataType::Float); });

    // Commit.
    expectNoThrow([&] { type0->commit(); });
    expectNoThrow([&] { type1->commit(); });
    expectNoThrow([&] { type2->commit(); });

    // Check committed.
    compareTrue(type0->isCommitted());
    compareTrue(type1->isCommitted());
    compareTrue(type2->isCommitted());

    // Commit again should throw.
    expectThrow([&] { type0->commit(); });
    expectThrow([&] { type1->commit(); });
    expectThrow([&] { type2->commit(); });

    // Adding another property should throw.
    expectThrow([&] { type0->createPrimitiveProperty("d", alex::DataType::Float); });
}
