#include "alexandria_test/create/create_primitive.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria/library.h"

namespace
{
    struct Foo
    {
        alex::InstanceId id;
        float            a = 0;
        double           b = 0;
    };

    struct Bar
    {
        alex::InstanceId id;
        int32_t          a = 0;
        int64_t          b = 0;
        uint32_t         c = 0;
        uint64_t         d = 0;
    };
}  // namespace

void CreatePrimitive::operator()()
{
    // Create type with floats.
    auto& fooType = library->createType("Foo");
    fooType.createPrimitiveProperty("floatProp", alex::DataType::Float).setDefaultValue(5.0f);
    fooType.createPrimitiveProperty("doubleProp", alex::DataType::Double).setDefaultValue(10.0);

    // Create type with integers.
    auto& barType = library->createType("Bar");
    barType.createPrimitiveProperty("int32Prop", alex::DataType::Int32).setDefaultValue(static_cast<int32_t>(-10));
    barType.createPrimitiveProperty("int64Prop", alex::DataType::Int64).setDefaultValue(static_cast<int64_t>(-20));
    barType.createPrimitiveProperty("uint32Prop", alex::DataType::Uint32).setDefaultValue(static_cast<uint32_t>(30));
    barType.createPrimitiveProperty("uint64Prop", alex::DataType::Uint64).setDefaultValue(static_cast<uint64_t>(40));

    // Commit types.
    expectNoThrow([this]() { library->commitTypes(); }).fatal("Failed to commit types");

    // Create object handlers.
    auto fooHandler = library->createObjectHandler<&Foo::id, &Foo::a, &Foo::b>(fooType.getName());
    auto barHandler = library->createObjectHandler<&Bar::id, &Bar::a, &Bar::b, &Bar::c, &Bar::d>(barType.getName());

    // Insert Foo.
    {
        std::unique_ptr<Foo> foo0, foo1;

        // Try to create with default values.
        expectNoThrow([&] { foo0 = fooHandler.create(); }).fatal("Failed to create object");
        expectNoThrow([&] { foo1 = fooHandler.create(); }).fatal("Failed to create object");

        // Compare objects.
        compareEQ(foo0->id, alex::InstanceId(1));
        compareEQ(foo0->a, 5.0f);
        compareEQ(foo0->b, 10.0);
        compareEQ(foo1->id, alex::InstanceId(2));
        compareEQ(foo1->a, 5.0f);
        compareEQ(foo1->b, 10.0);
    }

    // Insert Bar.
    {
        std::unique_ptr<Bar> bar0, bar1;

        // Try to create with default values.
        expectNoThrow([&] { bar0 = barHandler.create(); }).fatal("Failed to create object");
        expectNoThrow([&] { bar1 = barHandler.create(); }).fatal("Failed to create object");

        // Compare objects.
        compareEQ(bar0->id, alex::InstanceId(1));
        compareEQ(bar0->a, static_cast<int32_t>(-10));
        compareEQ(bar0->b, static_cast<int64_t>(-20));
        compareEQ(bar0->c, static_cast<uint32_t>(30));
        compareEQ(bar0->d, static_cast<uint64_t>(40));
        compareEQ(bar1->id, alex::InstanceId(2));
        compareEQ(bar1->a, static_cast<int32_t>(-10));
        compareEQ(bar1->b, static_cast<int64_t>(-20));
        compareEQ(bar1->c, static_cast<uint32_t>(30));
        compareEQ(bar1->d, static_cast<uint64_t>(40));
    }
}
