#include "alexandria_test/create_property_primitive.h"

void CreatePropertyPrimitive::operator()()
{
    // Create several properties.
    alex::Property *prop0, *prop1, *prop2, *prop3, *prop4, *prop5, *prop6, *prop7;
    expectNoThrow([&prop0, this]() { prop0 = &library->createPrimitiveProperty("prop0", alex::DataType::Int32); });
    expectNoThrow([&prop1, this]() { prop1 = &library->createPrimitiveProperty("prop1", alex::DataType::Float); });
    expectNoThrow([&prop2, this]() { prop2 = &library->createPrimitiveProperty("prop2", alex::DataType::String); });
    expectNoThrow(
      [&prop3, this]() { prop3 = &library->createPrimitiveArrayProperty("prop3", alex::DataType::Int32, false); });
    expectNoThrow(
      [&prop4, this]() { prop4 = &library->createPrimitiveArrayProperty("prop4", alex::DataType::Float, false); });
    expectNoThrow(
      [&prop5, this]() { prop5 = &library->createPrimitiveArrayProperty("prop5", alex::DataType::String, false); });
    expectNoThrow(
      [&prop6, this]() { prop6 = &library->createPrimitiveArrayProperty("prop6", alex::DataType::Int32, true); });
    expectNoThrow(
      [&prop7, this]() { prop7 = &library->createPrimitiveArrayProperty("prop7", alex::DataType::Float, true); });
    expectThrow([this]() { library->createPrimitiveArrayProperty("prop8", alex::DataType::String, true); });

    // Check props.

    compareEQ(prop0->getDataType(), alex::DataType::Int32);
    compareEQ(prop0->getNestedType(), nullptr);
    compareFalse(prop0->isReference());
    compareFalse(prop0->isArray());
    compareFalse(prop0->isBlob());

    compareEQ(prop1->getDataType(), alex::DataType::Float);
    compareEQ(prop1->getNestedType(), nullptr);
    compareFalse(prop1->isReference());
    compareFalse(prop1->isArray());
    compareFalse(prop1->isBlob());

    compareEQ(prop2->getDataType(), alex::DataType::String);
    compareEQ(prop2->getNestedType(), nullptr);
    compareFalse(prop2->isReference());
    compareFalse(prop2->isArray());
    compareFalse(prop2->isBlob());

    compareEQ(prop3->getDataType(), alex::DataType::Int32);
    compareEQ(prop3->getNestedType(), nullptr);
    compareTrue(prop3->isReference());
    compareTrue(prop3->isArray());
    compareFalse(prop3->isBlob());

    compareEQ(prop4->getDataType(), alex::DataType::Float);
    compareEQ(prop4->getNestedType(), nullptr);
    compareTrue(prop4->isReference());
    compareTrue(prop4->isArray());
    compareFalse(prop4->isBlob());

    compareEQ(prop5->getDataType(), alex::DataType::String);
    compareEQ(prop5->getNestedType(), nullptr);
    compareTrue(prop5->isReference());
    compareTrue(prop5->isArray());
    compareFalse(prop5->isBlob());

    compareEQ(prop6->getDataType(), alex::DataType::Int32);
    compareEQ(prop6->getNestedType(), nullptr);
    compareFalse(prop6->isReference());
    compareTrue(prop6->isArray());
    compareTrue(prop6->isBlob());

    compareEQ(prop7->getDataType(), alex::DataType::Float);
    compareEQ(prop7->getNestedType(), nullptr);
    compareFalse(prop7->isReference());
    compareTrue(prop7->isArray());
    compareTrue(prop7->isBlob());
}
