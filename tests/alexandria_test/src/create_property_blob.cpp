#include "alexandria_test/create_property_blob.h"

void CreatePropertyBlob::operator()()
{
    // Create several properties.
    alex::Property *prop0, *prop1;
    expectNoThrow([&prop0, this]() { prop0 = &library->createBlobProperty("prop0", false); });
    expectNoThrow([&prop1, this]() { prop1 = &library->createBlobProperty("prop1", true); });

    // Check first prop.
    compareEQ(prop0->getDataType(), alex::DataType::Blob);
    compareEQ(prop0->getNestedType(), nullptr);
    compareFalse(prop0->isReference());
    compareFalse(prop0->isArray());
    compareTrue(prop0->isBlob());

    // Check second prop.
    compareEQ(prop1->getDataType(), alex::DataType::Blob);
    compareEQ(prop1->getNestedType(), nullptr);
    compareTrue(prop1->isReference());
    compareTrue(prop1->isArray());
    compareTrue(prop1->isBlob());
}
