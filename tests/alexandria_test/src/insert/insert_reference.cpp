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
        alex::InstanceId id;
        float            a;
        int32_t          b;
    };

    struct Bar
    {
        alex::InstanceId     id;
        alex::Reference<Foo> foo;
    };

    struct Baz
    {
        alex::InstanceId     id;
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

    // Add properties to types.
    fooType.createPrimitiveProperty("floatProp", alex::DataType::Float);
    fooType.createPrimitiveProperty("int32Prop", alex::DataType::Int32);
    barType.createReferenceProperty("fooProp", fooType);
    bazType.createReferenceProperty("fooProp", fooType);
    bazType.createReferenceProperty("barProp", barType);

    // Commit types.
    expectNoThrow([this]() { library->commitTypes(); }).fatal("Failed to commit types");

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
    bar0.foo = foo0;
    bar1.foo = foo1;

    // Try to insert.
    expectNoThrow([&] { barHandler.insert(bar0); }).fatal("Failed to insert object");
    expectNoThrow([&] { barHandler.insert(bar1); }).fatal("Failed to insert object");

    // Check assigned IDs.
    compareEQ(bar0.id, alex::InstanceId(1));
    compareEQ(bar1.id, alex::InstanceId(2));

    // Select inserted object using sql.
    auto bar0_get = barTable.selectOne(barTable.col<0>() == bar0.id.get(), true)(false);
    auto bar1_get = barTable.selectOne(barTable.col<0>() == bar1.id.get(), true)(false);

    // Compare objects.
    compareEQ(bar0.id, std::get<0>(bar0_get));
    compareEQ(bar0.foo.getId(), std::get<1>(bar0_get));
    compareEQ(bar1.id, std::get<0>(bar1_get));
    compareEQ(bar1.foo.getId(), std::get<1>(bar1_get));

    // Insert Baz.
    //
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
    expectNoThrow([&] { bazHandler.insert(baz0); }).fatal("Failed to insert object");
    expectNoThrow([&] { bazHandler.insert(baz1); }).fatal("Failed to insert object");
    expectNoThrow([&] { bazHandler.insert(baz2); }).fatal("Failed to insert object");
    expectNoThrow([&] { bazHandler.insert(baz3); }).fatal("Failed to insert object");

    // Check assigned IDs.
    compareEQ(baz0.id, alex::InstanceId(1));
    compareEQ(baz1.id, alex::InstanceId(2));
    compareEQ(baz2.id, alex::InstanceId(3));
    compareEQ(baz3.id, alex::InstanceId(4));

    // Select inserted object using sql.
    auto baz0_get = bazTable.selectOne(bazTable.col<0>() == baz0.id.get(), true)(false);
    auto baz1_get = bazTable.selectOne(bazTable.col<0>() == baz1.id.get(), true)(false);
    auto baz2_get = bazTable.selectOne(bazTable.col<0>() == baz2.id.get(), true)(false);
    auto baz3_get = bazTable.selectOne(bazTable.col<0>() == baz3.id.get(), true)(false);

    // Compare objects.
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
