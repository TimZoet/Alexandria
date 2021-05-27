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

    // Insert a bunch of objects.
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

    // Check bar's foo references.
    {
        auto                 s = barFooTable.select<int64_t, 2>(barFooTable.col<1>() == bar0.id.get(), true);
        std::vector<int64_t> bar0_foos(s.begin(), s.end());

        compareEQ(bar0_foos.size(), 2).fatal("");
        compareEQ(bar0_foos[0], foo0.id.get());
        compareEQ(bar0_foos[1], foo1.id.get());

        s = barFooTable.select<int64_t, 2>(barFooTable.col<1>() == bar1.id.get(), true);
        std::vector<int64_t> bar1_foos(s.begin(), s.end());
        compareEQ(bar1_foos.size(), 0).fatal("");
    }

    // Check baz's foo references.
    {
        auto                 s = bazFooTable.select<int64_t, 2>(bazFooTable.col<1>() == baz.id.get(), true);
        std::vector<int64_t> baz_foos(s.begin(), s.end());

        compareEQ(baz_foos.size(), 2).fatal("");
        compareEQ(baz_foos[0], foo1.id.get());
        compareEQ(baz_foos[1], foo0.id.get());
    }

    // Delete foo0.
    expectNoThrow([&]
    {
        fooHandler->del(foo0.id);
    });

    // Check bar's foo references.
    {
        auto                 s = barFooTable.select<int64_t, 2>(barFooTable.col<1>() == bar0.id.get(), true);
        std::vector<int64_t> bar0_foos(s.begin(), s.end());

        compareEQ(bar0_foos.size(), 1).fatal("");
        compareEQ(bar0_foos[0], foo1.id.get());

        s = barFooTable.select<int64_t, 2>(barFooTable.col<1>() == bar1.id.get(), true);
        std::vector<int64_t> bar1_foos(s.begin(), s.end());
        compareEQ(bar1_foos.size(), 0).fatal("");
    }

    // Check baz's foo references.
    {
        auto                 s = bazFooTable.select<int64_t, 2>(bazFooTable.col<1>() == baz.id.get(), true);
        std::vector<int64_t> baz_foos(s.begin(), s.end());
        compareEQ(baz_foos.size(), 1).fatal("");
        compareEQ(baz_foos[0], foo1.id.get());
    }

    // Delete foo1.
    expectNoThrow([&] { fooHandler->del(foo1.id); });

    // Check bar's foo references.
    {
        auto                 s = barFooTable.select<int64_t, 2>(barFooTable.col<1>() == bar0.id.get(), true);
        std::vector<int64_t> bar0_foos(s.begin(), s.end());
        compareEQ(bar0_foos.size(), 0).fatal("");

        s = barFooTable.select<int64_t, 2>(barFooTable.col<1>() == bar1.id.get(), true);
        std::vector<int64_t> bar1_foos(s.begin(), s.end());
        compareEQ(bar1_foos.size(), 0).fatal("");
    }

    // Check baz's foo references.
    {
        auto                 s = bazFooTable.select<int64_t, 2>(bazFooTable.col<1>() == baz.id.get(), true);
        std::vector<int64_t> baz_foos(s.begin(), s.end());

        compareEQ(baz_foos.size(), 0).fatal("");
    }

    // Check baz's bar references.
    {
        auto                 s = bazBarTable.select<int64_t, 2>(bazBarTable.col<1>() == baz.id.get(), true);
        std::vector<int64_t> baz_bars(s.begin(), s.end());

        compareEQ(baz_bars.size(), 3).fatal("");
        compareEQ(baz_bars[0], bar0.id.get());
        compareEQ(baz_bars[1], bar1.id.get());
        compareEQ(baz_bars[2], bar0.id.get());
    }

    // Delete bar0.
    expectNoThrow([&] { barHandler->del(bar0.id); });

    // Check baz's bar references.
    {
        auto                 s = bazBarTable.select<int64_t, 2>(bazBarTable.col<1>() == baz.id.get(), true);
        std::vector<int64_t> baz_bars(s.begin(), s.end());

        compareEQ(baz_bars.size(), 1).fatal("");
        compareEQ(baz_bars[0], bar1.id.get());
    }

    // Delete bar1.
    expectNoThrow([&] { barHandler->del(bar1.id); });

    // Check baz's bar references.
    {
        auto                 s = bazBarTable.select<int64_t, 2>(bazBarTable.col<1>() == baz.id.get(), true);
        std::vector<int64_t> baz_bars(s.begin(), s.end());

        compareEQ(baz_bars.size(), 0).fatal("");
    }
}
