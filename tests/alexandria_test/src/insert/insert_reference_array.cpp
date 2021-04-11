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
    expectNoThrow([this]() { library->commitTypes(); });

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
    Foo foo0{.a = 0.5f, .b = 4};
    Foo foo1{.a = -0.5f, .b = -10};
    fooHandler.insert(foo0);
    fooHandler.insert(foo1);

    // Insert Bar.
    Bar bar0, bar1;
    bar0.foo.add(foo0);
    bar0.foo.add(foo1);
    barHandler.insert(bar0);
    barHandler.insert(bar1);
    // Check assigned IDs.
    compareEQ(bar0.id, static_cast<int64_t>(1));
    compareEQ(bar1.id, static_cast<int64_t>(2));
    // Select inserted object using sql and compare.
    auto bar0_get = barTable.selectOne(barTable.col<0>() == bar0.id, true)(false);
    auto bar1_get = barTable.selectOne(barTable.col<0>() == bar1.id, true)(false);
    compareEQ(bar0.id, std::get<0>(bar0_get));
    compareEQ(bar1.id, std::get<0>(bar1_get));
    // Select references in separate table.
    auto                             idparam    = bar0.id;
    auto                             foo_select = barFooTable.select<2>(barFooTable.col<1>() == &idparam, true);
    std::vector<std::tuple<int64_t>> foo_get(foo_select.begin(), foo_select.end());
    compareEQ(*bar0.foo.begin(), std::get<0>(foo_get[0]));
    compareEQ(*(bar0.foo.begin() + 1), std::get<0>(foo_get[1]));
    idparam = bar1.id;
    foo_get.assign(foo_select(true).begin(), foo_select.end());
    compareEQ(static_cast<uint64_t>(0), foo_get.size());

    // Insert Baz.
    Baz baz;
    baz.foo.add(foo1);
    baz.foo.add(foo0);
    baz.bar.add(bar0);
    baz.bar.add(bar1);
    baz.bar.add(bar0);
    bazHandler.insert(baz);
    // Check assigned IDs.
    compareEQ(baz.id, static_cast<int64_t>(1));
    // Select inserted object using sql and compare.
    auto baz_get = bazTable.selectOne(bazTable.col<0>() == baz.id, true)(false);
    compareEQ(baz.id, std::get<0>(baz_get));
    // Select references in separate table.
    idparam          = baz.id;
    auto foo_select2 = bazFooTable.select<2>(bazFooTable.col<1>() == &idparam, true);
    auto bar_select  = bazBarTable.select<2>(bazBarTable.col<1>() == &idparam, true);
    foo_get.assign(foo_select2.begin(), foo_select2.end());
    compareEQ(*baz.foo.begin(), std::get<0>(foo_get[0]));
    compareEQ(*(baz.foo.begin() + 1), std::get<0>(foo_get[1]));
    foo_get.assign(bar_select.begin(), bar_select.end());
    compareEQ(*baz.bar.begin(), std::get<0>(foo_get[0]));
    compareEQ(*(baz.bar.begin() + 1), std::get<0>(foo_get[1]));
    compareEQ(*(baz.bar.begin() + 2), std::get<0>(foo_get[2]));
}
