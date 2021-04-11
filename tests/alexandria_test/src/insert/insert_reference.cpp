#include "alexandria_test/insert/insert_reference.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria/library.h"
#include "alexandria/member_types/reference.h"

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
        int64_t              id = 0;
        alex::Reference<Foo> foo;
    };

    struct Baz
    {
        int64_t              id = 0;
        alex::Reference<Foo> foo;
        alex::Reference<Bar> bar;
    };
}  // namespace

void InsertReference::operator()()
{
    // Create types.
    auto& fooType = library->createType("Foo");
    auto& barType = library->createType("Bar");
    auto& bazType = library->createType("Baz");

    // Create all property types.
    auto& floatProp = library->createPrimitiveProperty("floatProp", alex::DataType::Float);
    auto& int32Prop = library->createPrimitiveProperty("int32Prop", alex::DataType::Int32);
    auto& fooProp   = library->createNestedProperty("fooProp", fooType, true);
    auto& barProp   = library->createNestedProperty("barProp", barType, true);

    // Add properties to types.
    fooType.addProperty(floatProp);
    fooType.addProperty(int32Prop);
    barType.addProperty(fooProp);
    bazType.addProperty(fooProp);
    bazType.addProperty(barProp);

    // Commit types.
    expectNoThrow([this]() { library->commitTypes(); });

    // Get tables.
    sql::ext::TypedTable<int64_t, int64_t>          barTable(library->getDatabase().getTable(barType.getName()));
    sql::ext::TypedTable<int64_t, int64_t, int64_t> bazTable(library->getDatabase().getTable(bazType.getName()));

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
    bar0.foo = foo0;
    bar1.foo = foo1;
    barHandler.insert(bar0);
    barHandler.insert(bar1);
    // Check assigned IDs.
    compareEQ(bar0.id, static_cast<int64_t>(1));
    compareEQ(bar1.id, static_cast<int64_t>(2));
    // Select inserted object using sql and compare.
    auto bar0_get = barTable.selectOne(barTable.col<0>() == bar0.id, true)(false);
    auto bar1_get = barTable.selectOne(barTable.col<0>() == bar1.id, true)(false);
    compareEQ(bar0.id, std::get<0>(bar0_get));
    compareEQ(bar0.foo.getId(), std::get<1>(bar0_get));
    compareEQ(bar1.id, std::get<0>(bar1_get));
    compareEQ(bar1.foo.getId(), std::get<1>(bar1_get));

    // Insert Baz.
    Baz baz0, baz1, baz2, baz3;
    baz0.foo = foo0;
    baz0.bar = bar0;
    baz1.foo = foo0;
    baz1.bar = bar1;
    baz2.foo = foo1;
    baz2.bar = bar0;
    baz3.foo = foo1;
    baz3.bar = bar1;
    bazHandler.insert(baz0);
    bazHandler.insert(baz1);
    bazHandler.insert(baz2);
    bazHandler.insert(baz3);
    // Check assigned IDs.
    compareEQ(baz0.id, static_cast<int64_t>(1));
    compareEQ(baz1.id, static_cast<int64_t>(2));
    compareEQ(baz2.id, static_cast<int64_t>(3));
    compareEQ(baz3.id, static_cast<int64_t>(4));
    // Select inserted object using sql and compare.
    auto baz0_get = bazTable.selectOne(bazTable.col<0>() == baz0.id, true)(false);
    auto baz1_get = bazTable.selectOne(bazTable.col<0>() == baz1.id, true)(false);
    auto baz2_get = bazTable.selectOne(bazTable.col<0>() == baz2.id, true)(false);
    auto baz3_get = bazTable.selectOne(bazTable.col<0>() == baz3.id, true)(false);
    compareEQ(baz0.id, std::get<0>(baz0_get));
    compareEQ(baz0.foo.getId(), std::get<1>(baz0_get));
    compareEQ(baz0.bar.getId(), std::get<2>(baz0_get));
    compareEQ(baz1.id, std::get<0>(baz1_get));
    compareEQ(baz1.foo.getId(), std::get<1>(baz1_get));
    compareEQ(baz1.bar.getId(), std::get<2>(baz1_get));
    compareEQ(baz2.id, std::get<0>(baz2_get));
    compareEQ(baz2.foo.getId(), std::get<1>(baz2_get));
    compareEQ(baz2.bar.getId(), std::get<2>(baz2_get));
    compareEQ(baz3.id, std::get<0>(baz3_get));
    compareEQ(baz3.foo.getId(), std::get<1>(baz3_get));
    compareEQ(baz3.bar.getId(), std::get<2>(baz3_get));
}
