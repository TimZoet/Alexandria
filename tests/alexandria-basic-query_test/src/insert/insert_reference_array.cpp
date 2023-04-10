#include "alexandria-basic-query_test/insert/insert_reference_array.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria-core/library.h"
#include "alexandria-core/type_descriptor.h"
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

void InsertReferenceArray::operator()()
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

    // Insert Bar.
    {
        const sql::TypedTable<sql::row_id, std::string, std::string> arrayTable(
          library->getDatabase().getTable("main_bar_fooprop"));

        auto inserter = alex::InsertQuery(BarDescriptor(barType));

        // Try to insert.
        expectNoThrow([&] { inserter(bar0); }).fatal("Failed to insert object");
        expectNoThrow([&] { inserter(bar1); }).fatal("Failed to insert object");

        // Check assigned IDs.
        compareTrue(bar0.id.valid());
        compareTrue(bar1.id.valid());

        // Check references in array table.
        std::string id;
        auto        stmt = arrayTable.selectAs<std::string, 2>()
                      .where(like(arrayTable.col<1>(), &id))
                      .orderBy(ascending(arrayTable.col<0>()))
                      .compile();
        id = bar0.id.getAsString();
        stmt.bind(sql::BindParameters::All);
        std::vector<alex::InstanceId> refs(stmt.begin(), stmt.end());
        compareEQ(bar0.foo.get(), refs);
        id = bar1.id.getAsString();
        stmt.bind(sql::BindParameters::All);
        refs.assign(stmt.begin(), stmt.end());
        compareEQ(bar1.foo.get(), refs);
    }

    // Insert Baz.
    {
        const sql::TypedTable<sql::row_id, std::string, std::string> array0Table(
          library->getDatabase().getTable("main_baz_fooprop"));
        const sql::TypedTable<sql::row_id, std::string, std::string> array1Table(
          library->getDatabase().getTable("main_baz_barprop"));

        auto inserter = alex::InsertQuery(BazDescriptor(bazType));

        // Create objects.
        Baz baz0;
        baz0.foo.add(foo1);
        baz0.foo.add(foo0);
        baz0.bar.add(bar0);
        baz0.bar.add(bar1);
        baz0.bar.add(bar0);
        Baz baz1;

        // Try to insert.
        expectNoThrow([&] { inserter(baz0); }).fatal("Failed to insert object");
        expectNoThrow([&] { inserter(baz1); }).fatal("Failed to insert object");

        // Check assigned IDs.
        compareTrue(baz0.id.valid());
        compareTrue(baz1.id.valid());

        // Check references in array table.
        std::string id;
        auto        stmt0 = array0Table.selectAs<std::string, 2>()
                       .where(like(array0Table.col<1>(), &id))
                       .orderBy(ascending(array0Table.col<0>()))
                       .compile();
        auto stmt1 = array1Table.selectAs<std::string, 2>()
                       .where(like(array1Table.col<1>(), &id))
                       .orderBy(ascending(array1Table.col<0>()))
                       .compile();
        id = baz0.id.getAsString();
        stmt0.bind(sql::BindParameters::All);
        stmt1.bind(sql::BindParameters::All);
        std::vector<alex::InstanceId> refs0(stmt0.begin(), stmt0.end());
        std::vector<alex::InstanceId> refs1(stmt1.begin(), stmt1.end());
        compareEQ(baz0.foo.get(), refs0);
        compareEQ(baz0.bar.get(), refs1);
        id = baz1.id.getAsString();
        stmt0.bind(sql::BindParameters::All);
        stmt1.bind(sql::BindParameters::All);
        refs0.assign(stmt0.begin(), stmt0.end());
        refs1.assign(stmt1.begin(), stmt1.end());
        compareEQ(baz1.foo.get(), refs0);
        compareEQ(baz1.bar.get(), refs1);
    }
}
