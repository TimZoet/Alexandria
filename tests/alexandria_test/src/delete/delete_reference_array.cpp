#include "alexandria_test/delete/delete_reference_array.h"

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

void DeleteReferenceArray::operator()()
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
    bar0.foo.add(foo0);
    bar0.foo.add(foo1);
    expectNoThrow([&] { barHandler->insert(bar0); }).fatal("Failed to insert object");
    expectNoThrow([&] { barHandler->insert(bar1); }).fatal("Failed to insert object");
    Baz baz;
    baz.foo.add(foo1);
    baz.foo.add(foo0);
    baz.bar.add(bar0);
    baz.bar.add(bar1);
    baz.bar.add(bar0);
    expectNoThrow([&] { bazHandler->insert(baz); }).fatal("Failed to insert object");

    // Delete objects one by one and check tables.
    compareEQ(static_cast<size_t>(1), bazTable.countAll()(true));
    compareEQ(static_cast<size_t>(2), bazFooTable.countAll()(true));
    compareEQ(static_cast<size_t>(3), bazBarTable.countAll()(true));
    expectNoThrow([&] { bazHandler->del(baz.id); });
    compareEQ(static_cast<size_t>(0), bazTable.countAll()(true));
    compareEQ(static_cast<size_t>(0), bazFooTable.countAll()(true));
    compareEQ(static_cast<size_t>(0), bazBarTable.countAll()(true));

    compareEQ(static_cast<size_t>(2), barTable.countAll()(true));
    compareEQ(static_cast<size_t>(2 + 0), barFooTable.countAll()(true));
    expectNoThrow([&] { barHandler->del(bar0.id); });
    compareEQ(static_cast<size_t>(1), barTable.countAll()(true));
    compareEQ(static_cast<size_t>(0), barFooTable.countAll()(true));
    expectNoThrow([&] { barHandler->del(bar1.id); });
    compareEQ(static_cast<size_t>(0), barTable.countAll()(true));
    compareEQ(static_cast<size_t>(0), barFooTable.countAll()(true));
}
