#include "alexandria_test/get/get_reference_array.h"

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

void GetReferenceArray::operator()()
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

    // Retrieve Foo.
    //
    // Create and insert objects.
    Foo foo0{.a = 0.5f, .b = 4};
    Foo foo1{.a = -0.5f, .b = -10};
    expectNoThrow([&] { fooHandler.insert(foo0); }).fatal("Failed to insert object");
    expectNoThrow([&] { fooHandler.insert(foo1); }).fatal("Failed to insert object");

    // Retrieve Bar.
    //
    // Create and insert objects.
    Bar bar0, bar1;
    bar0.foo.add(foo0);
    bar0.foo.add(foo1);
    expectNoThrow([&] { barHandler.insert(bar0); }).fatal("Failed to insert object");
    expectNoThrow([&] { barHandler.insert(bar1); }).fatal("Failed to insert object");

    // Try to retrieve objects.
    std::unique_ptr<Bar> bar0_get, bar1_get;
    expectNoThrow([&] { bar0_get = barHandler.get(bar0.id); }).fatal("Failed to get object");
    expectNoThrow([&] { bar1_get = barHandler.get(bar1.id); }).fatal("Failed to get object");

    // Compare objects.
    compareEQ(bar0.id, bar0_get->id);
    compareEQ(bar0.foo.get(), bar0_get->foo.get());
    compareEQ(bar1.id, bar1_get->id);
    compareEQ(bar1.foo.get(), bar1_get->foo.get());

    // Retrieve Baz.
    //
    // Create and insert objects.
    Baz baz;
    baz.foo.add(foo1);
    baz.foo.add(foo0);
    baz.bar.add(bar0);
    baz.bar.add(bar1);
    baz.bar.add(bar0);
    expectNoThrow([&] { bazHandler.insert(baz); }).fatal("Failed to insert object");

     // Try to retrieve objects.
    std::unique_ptr<Baz> baz_get;
    expectNoThrow([&] { baz_get = bazHandler.get(baz.id); }).fatal("Failed to get object");

    // Compare objects.
    compareEQ(baz.id, baz_get->id);
    compareEQ(baz.foo.get(), baz_get->foo.get());
    compareEQ(baz.bar.get(), baz_get->bar.get());
}
