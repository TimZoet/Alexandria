#include "alexandria_test/update/update_primitive.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria/core/type_descriptor.h"
#include "alexandria/queries/get_query.h"
#include "alexandria/queries/insert_query.h"
#include "alexandria/queries/update_query.h"

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

    using FooDescriptor =
      alex::GenerateTypeDescriptor<alex::Member<&Foo::id>, alex::Member<&Foo::a>, alex::Member<&Foo::b>>;

    using BarDescriptor = alex::GenerateTypeDescriptor<alex::Member<&Bar::id>,
                                                       alex::Member<&Bar::a>,
                                                       alex::Member<&Bar::b>,
                                                       alex::Member<&Bar::c>,
                                                       alex::Member<&Bar::d>>;
}  // namespace

void UpdatePrimitive::operator()()
{
    // Create type with floats.
    auto& fooType = nameSpace->createType("foo");
    fooType.createPrimitiveProperty("floatprop", alex::DataType::Float);
    fooType.createPrimitiveProperty("doubleprop", alex::DataType::Double);

    // Create type with integers.
    auto& barType = nameSpace->createType("bar");
    barType.createPrimitiveProperty("int32prop", alex::DataType::Int32);
    barType.createPrimitiveProperty("int64prop", alex::DataType::Int64);
    barType.createPrimitiveProperty("uint32prop", alex::DataType::Uint32);
    barType.createPrimitiveProperty("uint64prop", alex::DataType::Uint64);

    // Commit types.
    expectNoThrow([&] {
        fooType.commit();
        barType.commit();
    }).fatal("Failed to commit types");

    // Update Foo.
    {
        auto inserter = alex::InsertQuery(FooDescriptor(fooType));
        auto updater  = alex::UpdateQuery(FooDescriptor(fooType));
        auto getter   = alex::GetQuery(FooDescriptor(fooType));

        // Create objects.
        Foo foo0{.a = 0.5f, .b = 1.5};
        Foo foo1{.a = -0.5f, .b = -1.5};

        // Try to insert.
        expectNoThrow([&] { inserter(foo0); }).fatal("Failed to insert object");
        expectNoThrow([&] { inserter(foo1); }).fatal("Failed to insert object");

        // Modify objects.
        foo0.a = 33.0f;
        foo0.b = 44.0;
        foo1.a = 1.55f;
        foo1.b = 4.5;

        // Try to update.
        expectNoThrow([&] { updater(foo0); }).fatal("Failed to update object");
        expectNoThrow([&] { updater(foo1); }).fatal("Failed to update object");

        // Try to retrieve.
        Foo foo0_get{.id = foo0.id};
        Foo foo1_get{.id = foo1.id};
        expectNoThrow([&] { getter(foo0_get); }).fatal("Failed to retrieve object");
        expectNoThrow([&] { getter(foo1_get); }).fatal("Failed to retrieve object");

        // Compare objects.
        compareEQ(foo0.id, foo0_get.id);
        compareEQ(foo0.a, foo0_get.a);
        compareEQ(foo0.b, foo0_get.b);
        compareEQ(foo1.id, foo1_get.id);
        compareEQ(foo1.a, foo1_get.a);
        compareEQ(foo1.b, foo1_get.b);
    }

    // Update Bar.
    {
        auto inserter = alex::InsertQuery(BarDescriptor(barType));
        auto updater  = alex::UpdateQuery(BarDescriptor(barType));
        auto getter   = alex::GetQuery(BarDescriptor(barType));

        // Create objects.
        Bar bar0{.a = 1, .b = 2, .c = 3, .d = 4};
        Bar bar1{.a = -1, .b = -2, .c = 123456, .d = 1234567};

        // Try to insert.
        expectNoThrow([&] { inserter(bar0); }).fatal("Failed to insert object");
        expectNoThrow([&] { inserter(bar1); }).fatal("Failed to insert object");

        // Modify objects.
        bar0.a = 12;
        bar0.b = -14;
        bar0.c = 55;
        bar0.d = 9;
        bar1.a = 7876;
        bar1.b = 235;
        bar1.c = 900;
        bar1.d = 564;

        // Try to update.
        expectNoThrow([&] { updater(bar0); }).fatal("Failed to update object");
        expectNoThrow([&] { updater(bar1); }).fatal("Failed to update object");

        // Try to retrieve.
        Bar bar0_get{.id = bar0.id};
        Bar bar1_get{.id = bar1.id};
        expectNoThrow([&] { getter(bar0_get); }).fatal("Failed to retrieve object");
        expectNoThrow([&] { getter(bar1_get); }).fatal("Failed to retrieve object");

        // Compare objects.
        compareEQ(bar0.id, bar0_get.id);
        compareEQ(bar0.a, bar0_get.a);
        compareEQ(bar0.b, bar0_get.b);
        compareEQ(bar0.c, bar0_get.c);
        compareEQ(bar0.d, bar0_get.d);
        compareEQ(bar1.id, bar1_get.id);
        compareEQ(bar1.a, bar1_get.a);
        compareEQ(bar1.b, bar1_get.b);
        compareEQ(bar1.c, bar1_get.c);
        compareEQ(bar1.d, bar1_get.d);
    }
}
