#include "alexandria_test/insert/insert_reference_array.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria/library.h"
#include "alexandria/member_types/reference_array.h"

namespace
{
    struct Foo
    {
        int64_t id = 0;
        float   a;
        int32_t b;
    };

    struct Bar
    {
        int64_t                   id = 0;
        alex::ReferenceArray<Foo> foo;
    };

    struct Baz
    {
        int64_t                   id = 0;
        alex::ReferenceArray<Foo> foo;
        alex::ReferenceArray<Bar> bar;
    };
}  // namespace

void InsertReferenceArray::operator()()
{
    // Create types.
    auto& fooType = library->createType("Foo");
    auto& barType = library->createType("Bar");
    auto& bazType = library->createType("Baz");

    // Create all property types.
    auto& floatProp = library->createPrimitiveProperty("floatProp", alex::DataType::Float);
    auto& int32Prop = library->createPrimitiveProperty("int32Prop", alex::DataType::Int32);
    auto& fooProp   = library->createNestedArrayProperty("fooProp", fooType);
    auto& barProp   = library->createNestedArrayProperty("barProp", barType);

    // Add properties to types.
    fooType.addProperty(floatProp);
    fooType.addProperty(int32Prop);
    barType.addProperty(fooProp);
    bazType.addProperty(fooProp);
    bazType.addProperty(barProp);

    // Commit types.
    expectNoThrow([this]() { library->commitTypes(); }).fatal("Failed to commit types");

    // Get tables.
    sql::ext::TypedTable<int64_t>                   barTable(library->getDatabase().getTable(barType.getName()));
    sql::ext::TypedTable<int64_t>                   bazTable(library->getDatabase().getTable(bazType.getName()));
    sql::ext::TypedTable<int64_t, int64_t, int64_t> barFooTable(
      library->getDatabase().getTable(barType.getName() + "_fooProp"));
    sql::ext::TypedTable<int64_t, int64_t, int64_t> bazFooTable(
      library->getDatabase().getTable(bazType.getName() + "_fooProp"));
    sql::ext::TypedTable<int64_t, int64_t, int64_t> bazBarTable(
      library->getDatabase().getTable(bazType.getName() + "_barProp"));

    // Create object handlers.
    auto fooHandler = library->createObjectHandler<&Foo::id, &Foo::a, &Foo::b>(fooType.getName());
    auto barHandler = library->createObjectHandler<&Bar::id, &Bar::foo>(barType.getName());
    auto bazHandler = library->createObjectHandler<&Baz::id, &Baz::foo, &Baz::bar>(bazType.getName());

    // TODO: Once the way references to not yet inserted objects are handled is finalized, test that here as well.
    // TODO: Test insert of empty/null references.

    // Insert Foo.
    //
    // Create objects.
    Foo foo0{.a = 0.5f, .b = 4};
    Foo foo1{.a = -0.5f, .b = -10};

    // Try to insert.
    expectNoThrow([&] { fooHandler.insert(foo0); }).fatal("Failed to insert object");
    expectNoThrow([&] { fooHandler.insert(foo1); }).fatal("Failed to insert object");

    // Insert Bar.
    //
    // Create objects.
    Bar bar0, bar1;
    bar0.foo.add(foo0);
    bar0.foo.add(foo1);

    // Try to insert.
    expectNoThrow([&] { barHandler.insert(bar0); }).fatal("Failed to insert object");
    expectNoThrow([&] { barHandler.insert(bar1); }).fatal("Failed to insert object");

    // Check assigned IDs.
    compareEQ(bar0.id, static_cast<int64_t>(1));
    compareEQ(bar1.id, static_cast<int64_t>(2));

    // Select inserted object using sql.
    auto bar0_get = barTable.selectOne<int64_t>(barTable.col<0>() == bar0.id, true)(false);
    auto bar1_get = barTable.selectOne<int64_t>(barTable.col<0>() == bar1.id, true)(false);

    // Compare objects.
    compareEQ(bar0.id, bar0_get);
    compareEQ(bar1.id, bar1_get);

    // Select references in separate table.
    auto                 idparam    = bar0.id;
    auto                 foo_select = barFooTable.select<int64_t, 2>(barFooTable.col<1>() == &idparam, true);
    std::vector<int64_t> foo_get(foo_select.begin(), foo_select.end());
    compareEQ(bar0.foo.get(), foo_get);
    idparam = bar1.id;
    foo_get.assign(foo_select(true).begin(), foo_select.end());
    compareEQ(bar1.foo.get(), foo_get);

    // Insert Baz.
    //
    // Create objects.
    Baz baz;
    baz.foo.add(foo1);
    baz.foo.add(foo0);
    baz.bar.add(bar0);
    baz.bar.add(bar1);
    baz.bar.add(bar0);

    // Try to insert.
    expectNoThrow([&] { bazHandler.insert(baz); }).fatal("Failed to insert object");

    // Check assigned IDs.
    compareEQ(baz.id, static_cast<int64_t>(1));

    // Select inserted object using sql.
    auto baz_get = bazTable.selectOne<int64_t>(bazTable.col<0>() == baz.id, true)(false);

    // Compare objects.
    compareEQ(baz.id, baz_get);

    // Select references in separate table.
    idparam          = baz.id;
    auto foo_select2 = bazFooTable.select<int64_t, 2>(bazFooTable.col<1>() == &idparam, true);
    auto bar_select  = bazBarTable.select<int64_t, 2>(bazBarTable.col<1>() == &idparam, true);
    foo_get.assign(foo_select2.begin(), foo_select2.end());
    compareEQ(baz.foo.get(), foo_get);
    foo_get.assign(bar_select.begin(), bar_select.end());
    compareEQ(baz.bar.get(), foo_get);
}
