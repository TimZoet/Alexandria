#include "alexandria_test/type_add_property.h"

void TypeAddProperty::operator()()
{
    // Create several types and properties.
    alex::Type* type;
    expectNoThrow([&type, this]() { type = &library->createType("type"); });
    alex::Property *prop0, *prop1, *prop2;
    expectNoThrow([&prop0, this]() { prop0 = &library->createPrimitiveProperty("prop0", alex::DataType::Int32); });
    expectNoThrow([&prop1, this]() { prop1 = &library->createPrimitiveProperty("prop1", alex::DataType::Int32); });
    expectNoThrow([&prop2, this]() { prop2 = &library->createPrimitiveProperty("prop2", alex::DataType::Int32); });

    // Add properties to type. Adding same prop again should throw.
    expectNoThrow([&type, &prop0]() { type->addProperty(*prop0); });
    expectNoThrow([&type, &prop1]() { type->addProperty(*prop1); });
    expectNoThrow([&type, &prop2]() { type->addProperty(*prop2); });
    expectThrow([&type, &prop2]() { type->addProperty(*prop2); });

    // Check type's property list.
    compareEQ(type->getProperties().size(), 3).fatal("");
    compareEQ(type->getProperties()[0], prop0);
    compareEQ(type->getProperties()[1], prop1);
    compareEQ(type->getProperties()[2], prop2);
}
