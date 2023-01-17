#include "alexandria_test/get/get_string.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria/core/type_descriptor.h"
#include "alexandria/queries/get_query.h"
#include "alexandria/queries/insert_query.h"

namespace
{
    struct Foo
    {
        alex::InstanceId id;
        std::string      a;

        Foo() = default;
        explicit Foo(const alex::InstanceId& iid) : id(iid) {}
        Foo(const std::string& sid, std::string sa) : id(sid), a(std::move(sa)) {}
    };

    struct Bar
    {
        alex::InstanceId id;
        std::string      a;
        std::string      b;

        Bar() = default;
        explicit Bar(const alex::InstanceId& iid) : id(iid) {}
        Bar(const std::string& sid, std::string sa, std::string sb) : id(sid), a(std::move(sa)), b(std::move(sb)) {}
    };

    using FooDescriptor = alex::GenerateTypeDescriptor<alex::Member<&Foo::id>, alex::Member<&Foo::a>>;

    using BarDescriptor =
      alex::GenerateTypeDescriptor<alex::Member<&Bar::id>, alex::Member<&Bar::a>, alex::Member<&Bar::b>>;
}  // namespace

void GetString::operator()()
{
    // Create type with 1 string.
    auto& fooType = nameSpace->createType("foo");
    fooType.createStringProperty("prop1");

    // Create type with 2 strings.
    auto& barType = nameSpace->createType("bar");
    barType.createStringProperty("prop1");
    barType.createStringProperty("prop2");

    // Commit types.
    expectNoThrow([&] {
        fooType.commit();
        barType.commit();
    }).fatal("Failed to commit types");

    // Retrieve Foo.
    {
        auto inserter = alex::InsertQuery(FooDescriptor(fooType));
        auto getter   = alex::GetQuery(FooDescriptor(fooType));

        // Create objects.
        Foo foo0, foo1;
        foo0.a = "abc";
        foo1.a = "def";

        // Try to insert.
        expectNoThrow([&] { inserter(foo0); }).fatal("Failed to insert object");
        expectNoThrow([&] { inserter(foo1); }).fatal("Failed to insert object");

        // Try to retrieve.
        Foo foo0_get(foo0.id);
        Foo foo1_get(foo1.id);
        expectNoThrow([&] { getter(foo0_get); }).fatal("Failed to retrieve object");
        expectNoThrow([&] { getter(foo1_get); }).fatal("Failed to retrieve object");

        // Compare objects.
        compareEQ(foo0.id, foo0_get.id);
        compareEQ(foo0.a, foo0_get.a);
        compareEQ(foo1.id, foo1_get.id);
        compareEQ(foo1.a, foo1_get.a);
    }

    // Retrieve Bar.
    {
        auto inserter = alex::InsertQuery(BarDescriptor(barType));
        auto getter   = alex::GetQuery(BarDescriptor(barType));

        // Create objects.
        Bar bar0, bar1;
        bar0.a = "hijkl";
        bar0.b = "aaaa";
        bar1.a = "^%*&";
        bar1.b = "";

        // Try to insert.
        expectNoThrow([&] { inserter(bar0); }).fatal("Failed to insert object");
        expectNoThrow([&] { inserter(bar1); }).fatal("Failed to insert object");

        // Try to retrieve.
        Bar bar0_get(bar0.id);
        Bar bar1_get(bar1.id);
        expectNoThrow([&] { getter(bar0_get); }).fatal("Failed to retrieve object");
        expectNoThrow([&] { getter(bar1_get); }).fatal("Failed to retrieve object");

        // Compare objects.
        compareEQ(bar0.id, bar0_get.id);
        compareEQ(bar0.a, bar0_get.a);
        compareEQ(bar0.b, bar0_get.b);
        compareEQ(bar1.id, bar1_get.id);
        compareEQ(bar1.a, bar1_get.a);
        compareEQ(bar1.b, bar1_get.b);
    }
}
