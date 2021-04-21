#include "alexandria_test/get/get_reference.h"

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

void GetReference::operator()()
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
    bar0.foo = foo0;
    bar1.foo = foo1;
    expectNoThrow([&] { barHandler.insert(bar0); }).fatal("Failed to insert object");
    expectNoThrow([&] { barHandler.insert(bar1); }).fatal("Failed to insert object");

    // Try to retrieve objects.
    std::unique_ptr<Bar> bar0_get, bar1_get;
    expectNoThrow([&] { bar0_get = barHandler.get(bar0.id); }).fatal("Failed to get object");
    expectNoThrow([&] { bar1_get = barHandler.get(bar1.id); }).fatal("Failed to get object");

     // Compare objects.
    compareEQ(bar0.id, bar0_get->id);
    compareEQ(bar0.foo.getId(), bar0_get->foo.getId());
    compareEQ(bar1.id, bar1_get->id);
    compareEQ(bar1.foo.getId(), bar1_get->foo.getId());

    // Retrieve Baz.
    //
    // Create and insert objects.
    Baz baz0, baz1, baz2, baz3;
    baz0.foo = foo0;
    baz0.bar = bar0;
    baz1.foo = foo0;
    baz1.bar = bar1;
    baz2.foo = foo1;
    baz2.bar = bar0;
    baz3.foo = foo1;
    baz3.bar = bar1;
    expectNoThrow([&] { bazHandler.insert(baz0); }).fatal("Failed to insert object");
    expectNoThrow([&] { bazHandler.insert(baz1); }).fatal("Failed to insert object");
    expectNoThrow([&] { bazHandler.insert(baz2); }).fatal("Failed to insert object");
    expectNoThrow([&] { bazHandler.insert(baz3); }).fatal("Failed to insert object");

     // Try to retrieve objects.
    std::unique_ptr<Baz> baz0_get, baz1_get, baz2_get, baz3_get;
    expectNoThrow([&] { baz0_get = bazHandler.get(baz0.id); }).fatal("Failed to get object");
    expectNoThrow([&] { baz1_get = bazHandler.get(baz1.id); }).fatal("Failed to get object");
    expectNoThrow([&] { baz2_get = bazHandler.get(baz2.id); }).fatal("Failed to get object");
    expectNoThrow([&] { baz3_get = bazHandler.get(baz3.id); }).fatal("Failed to get object");

    // Compare objects.
    compareEQ(baz0.id, baz0_get->id);
    compareEQ(baz0.foo.getId(), baz0_get->foo.getId());
    compareEQ(baz0.bar.getId(), baz0_get->bar.getId());
    compareEQ(baz1.id, baz1_get->id);
    compareEQ(baz1.foo.getId(), baz1_get->foo.getId());
    compareEQ(baz1.bar.getId(), baz1_get->bar.getId());
    compareEQ(baz2.id, baz2_get->id);
    compareEQ(baz2.foo.getId(), baz2_get->foo.getId());
    compareEQ(baz2.bar.getId(), baz2_get->bar.getId());
    compareEQ(baz3.id, baz3_get->id);
    compareEQ(baz3.foo.getId(), baz3_get->foo.getId());
    compareEQ(baz3.bar.getId(), baz3_get->bar.getId());
}
