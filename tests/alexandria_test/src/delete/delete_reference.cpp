#include "alexandria_test/delete/delete_reference.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria-core/core/library.h"
#include "alexandria-core/core/type_descriptor.h"
#include "alexandria-query/queries/delete_query.h"
#include "alexandria-query/queries/insert_query.h"

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
        Bar(const std::string& iid, const std::string& fooid) : id(iid), foo(alex::InstanceId(fooid)) {}
    };

    struct Baz
    {
        alex::InstanceId     id;
        alex::Reference<Foo> foo;
        alex::Reference<Bar> bar;

        Baz() = default;
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

void DeleteReference::operator()()
{
    // Create types.
    auto& fooType = nameSpace->createType("foo");
    auto& barType = nameSpace->createType("bar");
    auto& bazType = nameSpace->createType("baz");

    // Add properties to types.
    fooType.createPrimitiveProperty("floatprop", alex::DataType::Float);
    fooType.createPrimitiveProperty("int32prop", alex::DataType::Int32);
    barType.createReferenceProperty("fooprop", fooType);
    bazType.createReferenceProperty("fooprop", fooType);
    bazType.createReferenceProperty("barprop", barType);

    // Commit types.
    expectNoThrow([&] {
        fooType.commit();
        barType.commit();
        bazType.commit();
    }).fatal("Failed to commit types");

    // Create Foo.
    Foo foo0{.a = 0.5f, .b = 4};
    Foo foo1{.a = -0.5f, .b = -10};
    expectNoThrow([&] {
        auto inserter = alex::InsertQuery(FooDescriptor(fooType));
        inserter(foo0);
        inserter(foo1);
    }).fatal("Failed to insert object");

    // Create Bar.
    Bar bar0, bar1;
    bar0.foo = foo0;
    bar1.foo = foo1;
    expectNoThrow([&] {
        auto inserter = alex::InsertQuery(BarDescriptor(barType));
        inserter(bar0);
        inserter(bar1);
    }).fatal("Failed to insert object");

    // Create Baz.
    Baz baz0, baz1, baz2, baz3, baz4;
    baz0.foo = foo0;
    baz0.bar = bar0;
    baz1.foo = foo0;
    baz1.bar = bar1;
    baz2.foo = foo1;
    baz2.bar = bar0;
    baz3.foo = foo1;
    baz3.bar = bar1;
    expectNoThrow([&] {
        auto inserter = alex::InsertQuery(BazDescriptor(bazType));
        inserter(baz0);
        inserter(baz1);
        inserter(baz2);
        inserter(baz3);
        inserter(baz4);
    }).fatal("Failed to insert object");

    // Delete Baz.
    {
        const sql::TypedTable<sql::row_id, std::string, std::string, std::string> table(
          library->getDatabase().getTable("main_baz"));
        auto deleter = alex::DeleteQuery(BazDescriptor(bazType));

        // Verify existence of objects before and after delete.
        std::string id;
        auto        stmt = table.count().where(like(table.col<1>(), &id)).compile();
        id               = baz0.id.getAsString();
        compareEQ(1, stmt.bind(sql::BindParameters::All)());
        expectNoThrow([&] { deleter(baz0); });
        compareEQ(0, stmt.bind(sql::BindParameters::All)());
        id = baz1.id.getAsString();
        compareEQ(1, stmt.bind(sql::BindParameters::All)());
        expectNoThrow([&] { deleter(baz1); });
        compareEQ(0, stmt.bind(sql::BindParameters::All)());
        id = baz2.id.getAsString();
        compareEQ(1, stmt.bind(sql::BindParameters::All)());
        expectNoThrow([&] { deleter(baz2); });
        compareEQ(0, stmt.bind(sql::BindParameters::All)());
        id = baz3.id.getAsString();
        compareEQ(1, stmt.bind(sql::BindParameters::All)());
        expectNoThrow([&] { deleter(baz3); });
        compareEQ(0, stmt.bind(sql::BindParameters::All)());
        id = baz4.id.getAsString();
        compareEQ(1, stmt.bind(sql::BindParameters::All)());
        expectNoThrow([&] { deleter(baz4); });
        compareEQ(0, stmt.bind(sql::BindParameters::All)());
    }

    // Delete Bar.
    {
        const sql::TypedTable<sql::row_id, std::string, std::string> table(library->getDatabase().getTable("main_bar"));
        auto deleter = alex::DeleteQuery(BarDescriptor(barType));

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
