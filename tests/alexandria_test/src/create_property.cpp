#include "alexandria_test/create_property.h"

void CreateProperty::operator()()
{
    // Create several properties.
    alex::Property *prop0, *prop1, *prop2;
    expectNoThrow([&prop0, this]() { prop0 = &library->createPrimitiveProperty("prop0", alex::DataType::Int32); });
    expectNoThrow([&prop1, this]() { prop1 = &library->createPrimitiveProperty("prop1", alex::DataType::Int32); });
    expectNoThrow([&prop2, this]() { prop2 = &library->createPrimitiveProperty("prop2", alex::DataType::Int32); });

    // Recreating props with same name should throw.
    expectThrow([ this]() {library->createPrimitiveProperty("prop0", alex::DataType::Int32); });
    expectThrow([ this]() {library->createPrimitiveProperty("prop1", alex::DataType::Int32); });
    expectThrow([ this]() {library->createPrimitiveProperty("prop2", alex::DataType::Int32); });

    // Check name and committed.
    compareEQ(prop0->getName(), "prop0");
    compareEQ(prop1->getName(), "prop1");
    compareEQ(prop2->getName(), "prop2");
    compareFalse(prop0->isCommitted());
    compareFalse(prop1->isCommitted());
    compareFalse(prop2->isCommitted());
}
