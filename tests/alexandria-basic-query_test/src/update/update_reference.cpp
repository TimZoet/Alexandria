#include "alexandria-basic-query_test/update/update_reference.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria-core/type_descriptor.h"
#include "alexandria-basic-query/get_query.h"
#include "alexandria-basic-query/insert_query.h"
#include "alexandria-basic-query/update_query.h"

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
        alex::InstanceId     id;
        alex::Reference<Foo> foo;

        Bar() = default;
        explicit Bar(const alex::InstanceId& iid) : id(iid) {}
        Bar(const std::string& iid, const std::string& fooid) : id(iid), foo(alex::InstanceId(fooid)) {}
    };

    struct Baz
    {
        alex::InstanceId     id;
        alex::Reference<Foo> foo;
        alex::Reference<Bar> bar;

        Baz() = default;
        explicit Baz(const std::string& iid) : id(iid) {}
        explicit Baz(const alex::InstanceId& iid) : id(iid) {}
        Baz(const std::string& iid, const std::string& fooid, const std::string& barid) :
            id(iid), foo(alex::InstanceId(fooid)), bar(alex::InstanceId(barid))
        {
        }
    };

    using FooDescriptor = alex::
      GenerateTypeDescriptor<alex::Member<"id", &Foo::id>, alex::Member<"a", &Foo::a>, alex::Member<"b", &Foo::b>>;

    using BarDescriptor = alex::GenerateTypeDescriptor<alex::Member<"id", &Bar::id>, alex::Member<"foo", &Bar::foo>>;

    using BazDescriptor = alex::GenerateTypeDescriptor<alex::Member<"id", &Baz::id>,
                                                       alex::Member<"foo", &Baz::foo>,
                                                       alex::Member<"bar", &Baz::bar>>;
}  // namespace

void UpdateReference::operator()()
{
    expectNoThrow([&] {
        alex::TypeLayout fooLayout;
        fooLayout.createPrimitiveProperty("prop0", alex::DataType::Float);
        fooLayout.createPrimitiveProperty("prop1", alex::DataType::Int32);
        fooLayout.commit(*nameSpace, "foo");

        alex::TypeLayout barLayout;
        barLayout.createReferenceProperty("prop0", nameSpace->getType("foo"));
        barLayout.commit(*nameSpace, "bar");

        alex::TypeLayout bazLayout;
        bazLayout.createReferenceProperty("prop0", nameSpace->getType("foo"));
        bazLayout.createReferenceProperty("prop1", nameSpace->getType("bar"));
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
    bar0.foo = foo0;
    bar1.foo = foo1;

    // Update Bar.
    {
        auto inserter = alex::InsertQuery(BarDescriptor(barType));
        auto updater  = alex::UpdateQuery(BarDescriptor(barType));
        auto getter   = alex::GetQuery(BarDescriptor(barType));

        // Try to insert.
        expectNoThrow([&] { inserter(bar0); }).fatal("Failed to insert object");
        expectNoThrow([&] { inserter(bar1); }).fatal("Failed to insert object");

        // Modify objects.
        bar0.foo = foo1;
        bar1.foo = foo0;

        // Try to update.
        expectNoThrow([&] { updater(bar0); }).fatal("Failed to update object");
        expectNoThrow([&] { updater(bar1); }).fatal("Failed to update object");

        // Try to retrieve.
        Bar bar0_get;
        Bar bar1_get;
        bar0_get.id = bar0.id;
        bar1_get.id = bar1.id;
        expectNoThrow([&] { getter(bar0_get); }).fatal("Failed to retrieve object");
        expectNoThrow([&] { getter(bar1_get); }).fatal("Failed to retrieve object");

        // Compare objects.
        compareEQ(bar0.id, bar0_get.id);
        compareEQ(bar0.foo.getId(), bar0_get.foo.getId());
        compareEQ(bar1.id, bar1_get.id);
        compareEQ(bar1.foo.getId(), bar1_get.foo.getId());
    }

    // Update Baz.
    {
        auto inserter = alex::InsertQuery(BazDescriptor(bazType));
        auto updater  = alex::UpdateQuery(BazDescriptor(bazType));
        auto getter   = alex::GetQuery(BazDescriptor(bazType));

        // Create objects.
        Baz baz0, baz1, baz2, baz3;
        baz0.foo = foo0;
        baz0.bar = bar0;
        baz1.foo = foo0;
        baz1.bar = bar1;
        baz2.foo = foo1;
        baz2.bar = bar0;
        baz3.foo = foo1;
        baz3.bar = bar1;

        // Try to insert.
        expectNoThrow([&] { inserter(baz0); }).fatal("Failed to insert object");
        expectNoThrow([&] { inserter(baz1); }).fatal("Failed to insert object");
        expectNoThrow([&] { inserter(baz2); }).fatal("Failed to insert object");
        expectNoThrow([&] { inserter(baz3); }).fatal("Failed to insert object");

        // Modify objects.
        baz0.foo = foo1;
        baz0.bar = bar1;
        baz1.bar = bar1;
        baz2.foo = foo0;

        // Try to update.
        expectNoThrow([&] { updater(baz0); }).fatal("Failed to update object");
        expectNoThrow([&] { updater(baz1); }).fatal("Failed to update object");
        expectNoThrow([&] { updater(baz2); }).fatal("Failed to update object");
        expectNoThrow([&] { updater(baz3); }).fatal("Failed to update object");

        // Try to retrieve.
        Baz baz0_get(baz0.id);
        Baz baz1_get(baz1.id);
        Baz baz2_get(baz2.id);
        Baz baz3_get(baz3.id);
        expectNoThrow([&] { getter(baz0_get); }).fatal("Failed to retrieve object");
        expectNoThrow([&] { getter(baz1_get); }).fatal("Failed to retrieve object");
        expectNoThrow([&] { getter(baz2_get); }).fatal("Failed to retrieve object");
        expectNoThrow([&] { getter(baz3_get); }).fatal("Failed to retrieve object");

        // Compare objects.
        compareEQ(baz0.id, baz0_get.id);
        compareEQ(baz0.foo.getId(), baz0_get.foo.getId());
        compareEQ(baz0.bar.getId(), baz0_get.bar.getId());
        compareEQ(baz1.id, baz1_get.id);
        compareEQ(baz1.foo.getId(), baz1_get.foo.getId());
        compareEQ(baz1.bar.getId(), baz1_get.bar.getId());
        compareEQ(baz2.id, baz2_get.id);
        compareEQ(baz2.foo.getId(), baz2_get.foo.getId());
        compareEQ(baz2.bar.getId(), baz2_get.bar.getId());
        compareEQ(baz3.id, baz3_get.id);
        compareEQ(baz3.foo.getId(), baz3_get.foo.getId());
        compareEQ(baz3.bar.getId(), baz3_get.bar.getId());
    }
}
