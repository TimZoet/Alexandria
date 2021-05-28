#include "alexandria_test/delete/delete_primitive_array.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria/library.h"
#include "alexandria/member_types/member.h"
#include "alexandria/member_types/primitive_array.h"

namespace
{
    struct Foo
    {
        alex::InstanceId            id;
        alex::PrimitiveArray<float> floats;

        Foo() = default;

        Foo(alex::InstanceId iid, std::vector<float> ffloats) : id(iid) { floats.get() = std::move(ffloats); }
    };

    struct Bar
    {
        alex::InstanceId              id;
        alex::PrimitiveArray<int32_t> ints;

        Bar() = default;

        Bar(alex::InstanceId iid, std::vector<int32_t> iints) : id(iid) { ints.get() = std::move(iints); }
    };

    struct Baz
    {
        alex::InstanceId               id;
        alex::PrimitiveArray<uint64_t> ints;
        alex::PrimitiveArray<double>   floats;

        Baz() = default;

        Baz(alex::InstanceId iid, std::vector<uint64_t> iints, std::vector<double> ffloats) : id(iid)
        {
            ints.get()   = std::move(iints);
            floats.get() = std::move(ffloats);
        }
    };
}  // namespace

void DeletePrimitiveArray::operator()()
{
    // Create type with floats.
    auto& fooType = library->createType("Foo");
    fooType.createPrimitiveArrayProperty("floats", alex::DataType::Float);

    // Create type with integers.
    auto& barType = library->createType("Bar");
    barType.createPrimitiveArrayProperty("ints", alex::DataType::Int32);

    // Create type with floats and integers.
    auto& bazType = library->createType("Baz");
    bazType.createPrimitiveArrayProperty("uints", alex::DataType::Uint64);
    bazType.createPrimitiveArrayProperty("doubles", alex::DataType::Double);

    // Commit types.
    expectNoThrow([this]() { library->commitTypes(); }).fatal("Failed to commit types");

    // Get tables.
    sql::ext::TypedTable<int64_t>                 fooTable(library->getDatabase().getTable(fooType.getName()));
    sql::ext::TypedTable<int64_t, int64_t, float> fooFloatsTable(
      library->getDatabase().getTable(fooType.getName() + "_floats"));
    sql::ext::TypedTable<int64_t>                   barTable(library->getDatabase().getTable(barType.getName()));
    sql::ext::TypedTable<int64_t, int64_t, int32_t> barIntsTable(
      library->getDatabase().getTable(barType.getName() + "_ints"));
    sql::ext::TypedTable<int64_t>                    bazTable(library->getDatabase().getTable(bazType.getName()));
    sql::ext::TypedTable<int64_t, int64_t, uint64_t> bazIntsTable(
      library->getDatabase().getTable(bazType.getName() + "_uints"));
    sql::ext::TypedTable<int64_t, int64_t, double> bazFloatsTable(
      library->getDatabase().getTable(bazType.getName() + "_doubles"));

    // Create object handlers.
    auto fooHandler =
      library->createObjectHandler<alex::Member<&Foo::id>, alex::Member<&Foo::floats>>(fooType.getName());
    auto barHandler = library->createObjectHandler<alex::Member<&Bar::id>, alex::Member<&Bar::ints>>(barType.getName());
    auto bazHandler =
      library->createObjectHandler<alex::Member<&Baz::id>, alex::Member<&Baz::ints>, alex::Member<&Baz::floats>>(
        bazType.getName());

    // Delete Foo.
    {
        // Create and insert objects.
        Foo foo0;
        foo0.floats.get().push_back(0.5f);
        foo0.floats.get().push_back(1.5f);
        Foo foo1;
        foo1.floats.get().push_back(-2.5f);
        foo1.floats.get().push_back(-3.5f);
        foo1.floats.get().push_back(-4.5f);
        expectNoThrow([&] { fooHandler->insert(foo0); }).fatal("Failed to insert object");
        expectNoThrow([&] { fooHandler->insert(foo1); }).fatal("Failed to insert object");

        // Delete objects one by one and check tables.
        compareEQ(static_cast<size_t>(2), fooTable.countAll()(true));
        compareEQ(static_cast<size_t>(2 + 3), fooFloatsTable.countAll()(true));
        expectNoThrow([&] { fooHandler->del(foo0.id); });
        compareEQ(static_cast<size_t>(1), fooTable.countAll()(true));
        compareEQ(static_cast<size_t>(3), fooFloatsTable.countAll()(true));
        expectNoThrow([&] { fooHandler->del(foo1.id); });
        compareEQ(static_cast<size_t>(0), fooTable.countAll()(true));
        compareEQ(static_cast<size_t>(0), fooFloatsTable.countAll()(true));
    }

    // Delete Bar.
    {
        // Create and insert objects.
        Bar bar0;
        bar0.ints.get().push_back(10);
        bar0.ints.get().push_back(100);
        Bar bar1;
        bar1.ints.get().push_back(-111);
        bar1.ints.get().push_back(-2222);
        bar1.ints.get().push_back(-33333);
        expectNoThrow([&] { barHandler->insert(bar0); }).fatal("Failed to insert object");
        expectNoThrow([&] { barHandler->insert(bar1); }).fatal("Failed to insert object");

        // Delete objects one by one and check tables.
        compareEQ(static_cast<size_t>(2), barTable.countAll()(true));
        compareEQ(static_cast<size_t>(2 + 3), barIntsTable.countAll()(true));
        expectNoThrow([&] { barHandler->del(bar0.id); });
        compareEQ(static_cast<size_t>(1), barTable.countAll()(true));
        compareEQ(static_cast<size_t>(3), barIntsTable.countAll()(true));
        expectNoThrow([&] { barHandler->del(bar1.id); });
        compareEQ(static_cast<size_t>(0), barTable.countAll()(true));
        compareEQ(static_cast<size_t>(0), barIntsTable.countAll()(true));
    }

    // Delete Baz.
    {
        // Create and insert objects.
        Baz baz0;
        baz0.ints.get().push_back(10);
        baz0.ints.get().push_back(100);
        baz0.floats.get().push_back(0.5);
        baz0.floats.get().push_back(1.5);
        Baz baz1;
        baz1.ints.get().push_back(-111);
        baz1.ints.get().push_back(-2222);
        baz1.ints.get().push_back(-33333);
        baz1.floats.get().push_back(-2.5);
        baz1.floats.get().push_back(-3.5);
        baz1.floats.get().push_back(-4.5);
        expectNoThrow([&] { bazHandler->insert(baz0); }).fatal("Failed to insert object");
        expectNoThrow([&] { bazHandler->insert(baz1); }).fatal("Failed to insert object");

        // Delete objects one by one and check tables.
        compareEQ(static_cast<size_t>(2), bazTable.countAll()(true));
        compareEQ(static_cast<size_t>(2 + 3), bazIntsTable.countAll()(true));
        compareEQ(static_cast<size_t>(2 + 3), bazFloatsTable.countAll()(true));
        expectNoThrow([&] { bazHandler->del(baz0.id); });
        compareEQ(static_cast<size_t>(1), bazTable.countAll()(true));
        compareEQ(static_cast<size_t>(3), bazIntsTable.countAll()(true));
        compareEQ(static_cast<size_t>(3), bazFloatsTable.countAll()(true));
        expectNoThrow([&] { bazHandler->del(baz1.id); });
        compareEQ(static_cast<size_t>(0), bazTable.countAll()(true));
        compareEQ(static_cast<size_t>(0), bazIntsTable.countAll()(true));
        compareEQ(static_cast<size_t>(0), bazFloatsTable.countAll()(true));
    }
}
