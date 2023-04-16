#include "alexandria-basic-query_test/delete/delete_string.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria-core/type_descriptor.h"
#include "alexandria-basic-query/delete_query.h"
#include "alexandria-basic-query/insert_query.h"

namespace
{
    struct Foo
    {
        alex::InstanceId id;
        std::string      a;

        Foo() = default;
        Foo(const std::string& sid, std::string sa) : id(sid), a(std::move(sa)) {}
    };

    struct Bar
    {
        alex::InstanceId id;
        std::string      a;
        std::string      b;

        Bar() = default;
        Bar(const std::string& sid, std::string sa, std::string sb) : id(sid), a(std::move(sa)), b(std::move(sb)) {}
    };

    using FooDescriptor = alex::GenerateTypeDescriptor<alex::Member<"id", &Foo::id>, alex::Member<"a", &Foo::a>>;

    using BarDescriptor = alex::
      GenerateTypeDescriptor<alex::Member<"id", &Bar::id>, alex::Member<"a", &Bar::a>, alex::Member<"b", &Bar::b>>;
}  // namespace

void DeleteString::operator()()
{
    expectNoThrow([&] {
        alex::TypeLayout fooLayout;
        fooLayout.createStringProperty("prop0");
        fooLayout.commit(*nameSpace, "foo");

        alex::TypeLayout barLayout;
        barLayout.createStringProperty("prop0");
        barLayout.createStringProperty("prop1");
        barLayout.commit(*nameSpace, "bar");
    }).fatal("Failed to commit types");

    auto& fooType = nameSpace->getType("foo");
    auto& barType = nameSpace->getType("bar");

    // Delete Foo.
    {
        const sql::TypedTable<sql::row_id, std::string, std::string> table(fooType.getInstanceTable());
        auto inserter = alex::InsertQuery(FooDescriptor(fooType));
        auto deleter  = alex::DeleteQuery(FooDescriptor(fooType));

        // Create objects.
        Foo foo0, foo1;
        foo0.a = "abc";
        foo1.a = "def";

        // Try to insert.
        expectNoThrow([&] { inserter(foo0); }).fatal("Failed to insert object");
        expectNoThrow([&] { inserter(foo1); }).fatal("Failed to insert object");

        // Verify existence of objects before and after delete.
        std::string id;
        auto        stmt = table.count().where(like(table.col<1>(), &id)).compile();
        id               = foo0.id.getAsString();
        compareEQ(1, stmt.bind(sql::BindParameters::All)());
        expectNoThrow([&] { deleter(foo0); });
        compareEQ(0, stmt.bind(sql::BindParameters::All)());
        id = foo1.id.getAsString();
        compareEQ(1, stmt.bind(sql::BindParameters::All)());
        expectNoThrow([&] { deleter(foo1); });
        compareEQ(0, stmt.bind(sql::BindParameters::All)());
    }

    // Delete Bar.
    {
        const sql::TypedTable<sql::row_id, std::string, std::string, std::string> table(barType.getInstanceTable());
        auto inserter = alex::InsertQuery(BarDescriptor(barType));
        auto deleter  = alex::DeleteQuery(BarDescriptor(barType));

        // Create objects.
        Bar bar0, bar1;
        bar0.a = "hijkl";
        bar0.b = "aaaa";
        bar1.a = "^%*&";
        bar1.b = "";

        // Try to insert.
        expectNoThrow([&] { inserter(bar0); }).fatal("Failed to insert object");
        expectNoThrow([&] { inserter(bar1); }).fatal("Failed to insert object");

        // Verify existence of objects before and after delete.
        std::string id;
        auto        stmt = table.count().where(like(table.col<1>(), &id)).compile();
        id               = bar0.id.getAsString();
        compareEQ(1, stmt.bind(sql::BindParameters::All)());
        expectNoThrow([&] { deleter(bar0); });
        compareEQ(0, stmt.bind(sql::BindParameters::All)());
        id = bar1.id.getAsString();
        compareEQ(1, stmt.bind(sql::BindParameters::All)());
        expectNoThrow([&] { deleter(bar1); });
        compareEQ(0, stmt.bind(sql::BindParameters::All)());
    }
}
