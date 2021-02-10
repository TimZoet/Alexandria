#include "alexandria_test/create_property_nested.h"

void CreatePropertyNested::operator()()
{
    // Create type.
    alex::Type *type;
    expectNoThrow([&type, this]() { type = &library->createType("type"); });

    // Create several properties.
    alex::Property *prop0, *prop1, *prop2;
    expectNoThrow([&prop0, &type, this]() { prop0 = &library->createNestedProperty("prop0", *type, false); });
    expectNoThrow([&prop1, &type, this]() { prop1 = &library->createNestedProperty("prop1", *type, true); });
    expectNoThrow([&prop2, &type, this]() { prop2 = &library->createNestedArrayProperty("prop2", *type); });

    // Check first prop.
    compareEQ(prop0->getDataType(), alex::DataType::NestedType);
    compareEQ(prop0->getNestedType(), type);
    compareFalse(prop0->isReference());
    compareFalse(prop0->isArray());
    compareFalse(prop0->isBlob());

    // Check second prop.
    compareEQ(prop1->getDataType(), alex::DataType::NestedType);
    compareEQ(prop1->getNestedType(), type);
    compareTrue(prop1->isReference());
    compareFalse(prop1->isArray());
    compareFalse(prop1->isBlob());

    // Check third prop.
    compareEQ(prop2->getDataType(), alex::DataType::NestedType);
    compareEQ(prop2->getNestedType(), type);
    compareTrue(prop2->isReference());
    compareTrue(prop2->isArray());
    compareFalse(prop2->isBlob());
}
