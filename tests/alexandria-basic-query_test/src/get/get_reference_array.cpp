#include "alexandria-basic-query_test/get/get_reference_array.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria-core/type_descriptor.h"
#include "alexandria-basic-query/get_query.h"
#include "alexandria-basic-query/insert_query.h"

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

void GetReferenceArray::operator()()
{
    expectNoThrow([&] {
        alex::TypeLayout fooLayout;
        fooLayout.createPrimitiveProperty("prop0", alex::DataType::Float);
        fooLayout.createPrimitiveProperty("prop1", alex::DataType::Int32);
        fooLayout.commit(*nameSpace, "foo");

        alex::TypeLayout barLayout;
        barLayout.createReferenceArrayProperty("prop0", nameSpace->getType("foo"));
        barLayout.commit(*nameSpace, "bar");

        alex::TypeLayout bazLayout;
        bazLayout.createReferenceArrayProperty("prop0", nameSpace->getType("foo"));
        bazLayout.createReferenceArrayProperty("prop1", nameSpace->getType("bar"));
        bazLayout.commit(*nameSpace, "baz");
    }).fatal("Failed to commit types");

    auto& fooType = nameSpace->getType("foo");
    auto& barType = nameSpace->getType("bar");
    auto& bazType = nameSpace->getType("baz");

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

    // Retrieve Bar.
    {
        auto inserter = alex::InsertQuery(BarDescriptor(barType));
        auto getter   = alex::GetQuery(BarDescriptor(barType));

        // Try to insert.
        expectNoThrow([&] { inserter(bar0); }).fatal("Failed to insert object");
        expectNoThrow([&] { inserter(bar1); }).fatal("Failed to insert object");

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

    // Retrieve Baz.
    {
        auto inserter = alex::InsertQuery(BazDescriptor(bazType));
        auto getter   = alex::GetQuery(BazDescriptor(bazType));

        // Create objects.
        Baz baz0, baz1;
        baz0.foo.add(foo1);
        baz0.foo.add(foo0);
        baz0.bar.add(bar0);
        baz0.bar.add(bar1);
        baz0.bar.add(bar0);

        // Try to insert.
        expectNoThrow([&] { inserter(baz0); }).fatal("Failed to insert object");
        expectNoThrow([&] { inserter(baz1); }).fatal("Failed to insert object");

        // Try to retrieve.
        Baz baz0_get, baz1_get;
        baz0_get.id = baz0.id;
        baz1_get.id = baz1.id;
        expectNoThrow([&] { getter(baz0_get); }).fatal("Failed to retrieve object");
        expectNoThrow([&] { getter(baz1_get); }).fatal("Failed to retrieve object");

        // Compare objects.
        compareEQ(baz0.foo.get(), baz0_get.foo.get());
        compareEQ(baz0.bar.get(), baz0_get.bar.get());
        compareEQ(baz1.foo.get(), baz1_get.foo.get());
        compareEQ(baz1.bar.get(), baz1_get.bar.get());
    }
}
