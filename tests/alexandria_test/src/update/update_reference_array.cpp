#include "alexandria_test/update/update_reference_array.h"

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
        float            a;
        int32_t          b;
    };

    struct Bar
    {
        alex::InstanceId          id;
        alex::ReferenceArray<Foo> foo;
    };

    struct Baz
    {
        alex::InstanceId          id;
        alex::ReferenceArray<Foo> foo;
        alex::ReferenceArray<Bar> bar;
    };

    using FooDescriptor = alex::
      GenerateTypeDescriptor<alex::Member<"id", &Foo::id>, alex::Member<"a", &Foo::a>, alex::Member<"b", &Foo::b>>;

    using BarDescriptor = alex::GenerateTypeDescriptor<alex::Member<"id", &Bar::id>, alex::Member<"foo", &Bar::foo>>;

    using BazDescriptor = alex::GenerateTypeDescriptor<alex::Member<"id", &Baz::id>,
                                                       alex::Member<"foo", &Baz::foo>,
                                                       alex::Member<"bar", &Baz::bar>>;
}  // namespace

void UpdateReferenceArray::operator()()
{
    // Create types.
    auto& fooType = nameSpace->createType("foo");
    auto& barType = nameSpace->createType("bar");
    auto& bazType = nameSpace->createType("baz");

    // Add properties to types.
    fooType.createPrimitiveProperty("floatprop", alex::DataType::Float);
    fooType.createPrimitiveProperty("int32prop", alex::DataType::Int32);
    barType.createReferenceArrayProperty("fooprop", fooType);
    bazType.createReferenceArrayProperty("fooprop", fooType);
    bazType.createReferenceArrayProperty("barprop", barType);

    // Commit types.
    expectNoThrow([&] {
        fooType.commit();
        barType.commit();
        bazType.commit();
    }).fatal("Failed to commit types");

    // Create objects.
    Foo foo0{.a = 0.5f, .b = 4};
    Foo foo1{.a = -0.5f, .b = -10};

    // Insert Foo.
    expectNoThrow([&] {
        auto inserter = alex::InsertQuery(FooDescriptor(fooType));
        inserter(foo0);
        inserter(foo1);
    }).fatal("Failed to insert object");

    Bar bar0, bar1;
    bar0.foo.add(foo0);
    bar0.foo.add(foo1);

    // Update Bar.
    {
        auto inserter = alex::InsertQuery(BarDescriptor(barType));
        auto updater  = alex::UpdateQuery(BarDescriptor(barType));
        auto getter   = alex::GetQuery(BarDescriptor(barType));

        // Try to insert.
        expectNoThrow([&] { inserter(bar0); }).fatal("Failed to insert object");
        expectNoThrow([&] { inserter(bar1); }).fatal("Failed to insert object");

        // Modify objects.
        bar0.foo.get().clear();
        bar1.foo.add(foo1);

        // Try to update.
        expectNoThrow([&] { updater(bar0); }).fatal("Failed to update object");
        expectNoThrow([&] { updater(bar1); }).fatal("Failed to update object");

        // Try to retrieve.
        Bar bar0_get, bar1_get;
        bar0_get.id = bar0.id;
        bar1_get.id = bar1.id;
        expectNoThrow([&] { getter(bar0_get); }).fatal("Failed to retrieve object");
        expectNoThrow([&] { getter(bar1_get); }).fatal("Failed to retrieve object");

        // Compare objects.
        compareEQ(bar0.foo.get(), bar0_get.foo.get());
        compareEQ(bar1.foo.get(), bar1_get.foo.get());
    }

    // Update Baz.
    {
        auto inserter = alex::InsertQuery(BazDescriptor(bazType));
        auto updater  = alex::UpdateQuery(BazDescriptor(bazType));
        auto getter   = alex::GetQuery(BazDescriptor(bazType));

        // Create objects.
        Baz baz;
        baz.foo.add(foo1);
        baz.foo.add(foo0);
        baz.bar.add(bar0);
        baz.bar.add(bar1);
        baz.bar.add(bar0);

        // Try to insert.
        expectNoThrow([&] { inserter(baz); }).fatal("Failed to insert object");

        // Modify objects.
        baz.foo.get().clear();
        baz.foo.add(foo0);
        baz.foo.add(foo0);
        baz.bar.add(bar1);

        // Try to update.
        expectNoThrow([&] { updater(baz); }).fatal("Failed to update object");

        // Try to retrieve.
        Baz baz_get;
        baz_get.id = baz.id;
        expectNoThrow([&] { getter(baz_get); }).fatal("Failed to retrieve object");

        // Compare objects.
        compareEQ(baz.foo.get(), baz_get.foo.get());
        compareEQ(baz.bar.get(), baz_get.bar.get());
    }
}
