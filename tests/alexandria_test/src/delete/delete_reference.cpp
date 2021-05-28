#include "alexandria_test/delete/delete_reference.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria/library.h"
#include "alexandria/member_types/member.h"
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

void DeleteReference::operator()()
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
    auto fooHandler =
      library->createObjectHandler<alex::Member<&Foo::id>, alex::Member<&Foo::a>, alex::Member<&Foo::b>>(
        fooType.getName());
    auto barHandler = library->createObjectHandler<alex::Member<&Bar::id>, alex::Member<&Bar::foo>>(barType.getName());
    auto bazHandler =
      library->createObjectHandler<alex::Member<&Baz::id>, alex::Member<&Baz::foo>, alex::Member<&Baz::bar>>(
        bazType.getName());

    // Create and insert objects.
    Foo foo0{.a = 0.5f, .b = 4};
    Foo foo1{.a = -0.5f, .b = -10};
    expectNoThrow([&] { fooHandler->insert(foo0); }).fatal("Failed to insert object");
    expectNoThrow([&] { fooHandler->insert(foo1); }).fatal("Failed to insert object");
    Bar bar0, bar1;
    bar0.foo = foo0;
    bar1.foo = foo1;
    expectNoThrow([&] { barHandler->insert(bar0); }).fatal("Failed to insert object");
    expectNoThrow([&] { barHandler->insert(bar1); }).fatal("Failed to insert object");
    Baz baz0, baz1, baz2, baz3;
    baz0.foo = foo0;
    baz0.bar = bar0;
    baz1.foo = foo0;
    baz1.bar = bar1;
    baz2.foo = foo1;
    baz2.bar = bar0;
    baz3.foo = foo1;
    baz3.bar = bar1;
    expectNoThrow([&] { bazHandler->insert(baz0); }).fatal("Failed to insert object");
    expectNoThrow([&] { bazHandler->insert(baz1); }).fatal("Failed to insert object");
    expectNoThrow([&] { bazHandler->insert(baz2); }).fatal("Failed to insert object");
    expectNoThrow([&] { bazHandler->insert(baz3); }).fatal("Failed to insert object");

    // Delete objects one by one and check tables.
    compareEQ(static_cast<size_t>(2), barTable.countAll()(true));
    expectNoThrow([&] { barHandler->del(bar0.id); });
    compareEQ(static_cast<size_t>(1), barTable.countAll()(true));
    expectNoThrow([&] { barHandler->del(bar1.id); });
    compareEQ(static_cast<size_t>(0), barTable.countAll()(true));

    compareEQ(static_cast<size_t>(4), bazTable.countAll()(true));
    expectNoThrow([&] { bazHandler->del(baz0.id); });
    compareEQ(static_cast<size_t>(3), bazTable.countAll()(true));
    expectNoThrow([&] { bazHandler->del(baz1.id); });
    compareEQ(static_cast<size_t>(2), bazTable.countAll()(true));
    expectNoThrow([&] { bazHandler->del(baz2.id); });
    compareEQ(static_cast<size_t>(1), bazTable.countAll()(true));
    expectNoThrow([&] { bazHandler->del(baz3.id); });
    compareEQ(static_cast<size_t>(0), bazTable.countAll()(true));
}
