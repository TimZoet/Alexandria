#include "alexandria_test/get/get_reference_array.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria/library.h"
#include "alexandria/member_types/member.h"
#include "alexandria/member_types/reference_array.h"

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
}  // namespace

void GetReferenceArray::operator()()
{
    // Create types.
    auto& fooType = library->createType("Foo");
    auto& barType = library->createType("Bar");
    auto& bazType = library->createType("Baz");

    // Add properties to types.
    fooType.createPrimitiveProperty("floatProp", alex::DataType::Float);
    fooType.createPrimitiveProperty("int32Prop", alex::DataType::Int32);
    barType.createReferenceArrayProperty("fooProp", fooType);
    bazType.createReferenceArrayProperty("fooProp", fooType);
    bazType.createReferenceArrayProperty("barProp", barType);

    // Commit types.
    expectNoThrow([this]() { library->commitTypes(); }).fatal("Failed to commit types");

    // Create object handlers.
    auto fooHandler =
      library->createObjectHandler<alex::Member<&Foo::id>, alex::Member<&Foo::a>, alex::Member<&Foo::b>>(
        fooType.getName());
    auto barHandler = library->createObjectHandler<alex::Member<&Bar::id>, alex::Member<&Bar::foo>>(barType.getName());
    auto bazHandler =
      library->createObjectHandler<alex::Member<&Baz::id>, alex::Member<&Baz::foo>, alex::Member<&Baz::bar>>(
        bazType.getName());

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
