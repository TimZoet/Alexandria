#include "alexandria_test/insert/insert_reference_array.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria/core/library.h"
#include "alexandria/queries/insert_query.h"

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

    using FooDescriptor =
      alex::GenerateTypeDescriptor<alex::Member<&Foo::id>, alex::Member<&Foo::a>, alex::Member<&Foo::b>>;

    using BarDescriptor = alex::GenerateTypeDescriptor<alex::Member<&Bar::id>, alex::Member<&Bar::foo>>;

    using BazDescriptor =
      alex::GenerateTypeDescriptor<alex::Member<&Baz::id>, alex::Member<&Baz::foo>, alex::Member<&Baz::bar>>;
}  // namespace

void InsertReferenceArray::operator()()
{
    // Create types.
    auto& fooType = nameSpace->createType("Foo");
    auto& barType = nameSpace->createType("Bar");
    auto& bazType = nameSpace->createType("Baz");

    // Add properties to types.
    fooType.createPrimitiveProperty("floatProp", alex::DataType::Float);
    fooType.createPrimitiveProperty("int32Prop", alex::DataType::Int32);
    barType.createReferenceArrayProperty("fooProp", fooType);
    bazType.createReferenceArrayProperty("fooProp", fooType);
    bazType.createReferenceArrayProperty("barProp", barType);

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
          library->getDatabase().getTable("main_Bar_fooProp"));

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

    {
        const sql::TypedTable<sql::row_id, std::string, std::string> arrayTable(
          library->getDatabase().getTable("main_Baz_barProp"));

        auto inserter = alex::InsertQuery(BazDescriptor(bazType));

        // Create objects.
        Baz baz;
        baz.foo.add(foo1);
        baz.foo.add(foo0);
        baz.bar.add(bar0);
        baz.bar.add(bar1);
        baz.bar.add(bar0);

        // Try to insert.
        expectNoThrow([&] { inserter(baz); }).fatal("Failed to insert object");

        // Check assigned IDs.
        compareTrue(baz.id.valid());

        // Check references in array table.
        auto stmt = arrayTable.selectAs<std::string, 2>()
                      .where(like(arrayTable.col<1>(), baz.id.getAsString()))
                      .orderBy(ascending(arrayTable.col<0>()))
                      .compile()
                      .bind(sql::BindParameters::All);
        const std::vector<alex::InstanceId> refs(stmt.begin(), stmt.end());
        compareEQ(baz.bar.get(), refs);
    }

    // TODO: Once the way references to not yet inserted objects are handled is finalized, test that here as well.
    // TODO: Test insert of empty/null references.
}
