#include "alexandria_test/create/create_primitive.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria/library.h"
#include "alexandria/member_types/member.h"

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
    auto fooHandler =
      library->createObjectHandler<alex::Member<&Foo::id>, alex::Member<&Foo::a>, alex::Member<&Foo::b>>(fooType);
    auto barHandler = library->createObjectHandler<alex::Member<&Bar::id>,
                                                   alex::Member<&Bar::a>,
                                                   alex::Member<&Bar::b>,
                                                   alex::Member<&Bar::c>,
                                                   alex::Member<&Bar::d>>(barType.getName());

    // Create Foo.
    {
        Foo foo0, foo1;

        // Try to create with default values.
        expectNoThrow([&] { fooHandler->create(&foo0); }).fatal("Failed to create object");
        expectNoThrow([&] { fooHandler->create(&foo1); }).fatal("Failed to create object");

        // Compare objects.
        compareEQ(foo0.id, alex::InstanceId(1));
        compareEQ(foo0.a, 5.0f);
        compareEQ(foo0.b, 10.0);
        compareEQ(foo1.id, alex::InstanceId(2));
        compareEQ(foo1.a, 5.0f);
        compareEQ(foo1.b, 10.0);
    }

    // Create Bar.
    {
        Bar bar0, bar1;

        // Try to create with default values.
        expectNoThrow([&] { barHandler->create(&bar0); }).fatal("Failed to create object");
        expectNoThrow([&] { barHandler->create(&bar1); }).fatal("Failed to create object");

        // Compare objects.
        compareEQ(bar0.id, alex::InstanceId(1));
        compareEQ(bar0.a, static_cast<int32_t>(-10));
        compareEQ(bar0.b, static_cast<int64_t>(-20));
        compareEQ(bar0.c, static_cast<uint32_t>(30));
        compareEQ(bar0.d, static_cast<uint64_t>(40));
        compareEQ(bar1.id, alex::InstanceId(2));
        compareEQ(bar1.a, static_cast<int32_t>(-10));
        compareEQ(bar1.b, static_cast<int64_t>(-20));
        compareEQ(bar1.c, static_cast<uint32_t>(30));
        compareEQ(bar1.d, static_cast<uint64_t>(40));
    }
}
