#include "alexandria-basic-query_test/delete/delete_reference_array.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria-core/library.h"
#include "alexandria-core/type_descriptor.h"
#include "alexandria-basic-query/delete_query.h"
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

void DeleteReferenceArray::operator()()
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
    bar0.foo.add(foo0);
    bar0.foo.add(foo1);
    expectNoThrow([&] {
        auto inserter = alex::InsertQuery(BarDescriptor(barType));
        inserter(bar0);
        inserter(bar1);
    }).fatal("Failed to insert object");

    // Create Baz.
    Baz baz0;
    baz0.foo.add(foo1);
    baz0.foo.add(foo0);
    baz0.bar.add(bar0);
    baz0.bar.add(bar1);
    baz0.bar.add(bar0);
    Baz baz1;
    expectNoThrow([&] {
        auto inserter = alex::InsertQuery(BazDescriptor(bazType));
        inserter(baz0);
        inserter(baz1);
    }).fatal("Failed to insert object");

    // Delete Baz.
    {
        const sql::TypedTable<sql::row_id, std::string, std::string> array0Table(
          library->getDatabase().getTable("main_baz_prop0"));
        const sql::TypedTable<sql::row_id, std::string, std::string> array1Table(
          library->getDatabase().getTable("main_baz_prop1"));

        auto deleter = alex::DeleteQuery(BazDescriptor(bazType));

        // Verify existence of objects before and after delete.
        std::string id;
        auto        stmt0 = array0Table.count().where(like(array0Table.col<1>(), &id)).compile();
        auto        stmt1 = array1Table.count().where(like(array1Table.col<1>(), &id)).compile();
        id                = baz0.id.getAsString();
        stmt0.bind(sql::BindParameters::All);
        stmt1.bind(sql::BindParameters::All);
        compareEQ(2, stmt0());
        compareEQ(3, stmt1());
        expectNoThrow([&] { deleter(baz0); });
        compareEQ(0, stmt0());
        compareEQ(0, stmt1());
        id = baz1.id.getAsString();
        stmt0.bind(sql::BindParameters::All);
        stmt1.bind(sql::BindParameters::All);
        compareEQ(0, stmt0());
        compareEQ(0, stmt1());
        expectNoThrow([&] { deleter(baz1); });
        compareEQ(0, stmt0());
        compareEQ(0, stmt1());
    }

    // Delete Bar.
    {
        const sql::TypedTable<sql::row_id, std::string, std::string> arrayTable(
          library->getDatabase().getTable("main_bar_prop0"));
        auto deleter = alex::DeleteQuery(BarDescriptor(barType));

        // Verify existence of objects before and after delete.
        std::string id;
        auto        stmt = arrayTable.count().where(like(arrayTable.col<1>(), &id)).compile();
        id               = bar0.id.getAsString();
        compareEQ(2, stmt.bind(sql::BindParameters::All)());
        expectNoThrow([&] { deleter(bar0); });
        compareEQ(0, stmt.bind(sql::BindParameters::All)());
        id = bar1.id.getAsString();
        compareEQ(0, stmt.bind(sql::BindParameters::All)());
        expectNoThrow([&] { deleter(bar1); });
        compareEQ(0, stmt.bind(sql::BindParameters::All)());
    }
}
