#include "alexandria_test/insert/insert_primitive_blob.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria/library.h"
#include "alexandria/member_types/member.h"
#include "alexandria/member_types/primitive_blob.h"

namespace
{
    struct Foo
    {
        alex::InstanceId           id;
        alex::PrimitiveBlob<float> floats;

        Foo() = default;

        Foo(alex::InstanceId iid, std::vector<float> ffloats) : id(iid) { floats.get() = std::move(ffloats); }
    };

    struct Bar
    {
        alex::InstanceId             id;
        alex::PrimitiveBlob<int32_t> ints;

        Bar() = default;

        Bar(alex::InstanceId iid, std::vector<int32_t> iints) : id(iid) { ints.get() = std::move(iints); }
    };

    struct Baz
    {
        alex::InstanceId              id;
        alex::PrimitiveBlob<uint64_t> ints;
        alex::PrimitiveBlob<double>   floats;

        Baz() = default;

        Baz(alex::InstanceId iid, std::vector<uint64_t> iints, std::vector<double> ffloats) : id(iid)
        {
            ints.get()   = std::move(iints);
            floats.get() = std::move(ffloats);
        }
    };
}  // namespace

void InsertPrimitiveBlob::operator()()
{
    // Create type with floats.
    auto& fooType = library->createType("Foo");
    fooType.createPrimitiveBlobProperty("floats", alex::DataType::Float);

    // Create type with integers.
    auto& barType = library->createType("Bar");
    barType.createPrimitiveBlobProperty("ints", alex::DataType::Int32);

    // Create type with floats and integers.
    auto& bazType = library->createType("Baz");
    bazType.createPrimitiveBlobProperty("uints", alex::DataType::Uint64);
    bazType.createPrimitiveBlobProperty("doubles", alex::DataType::Double);

    // Commit types.
    expectNoThrow([this]() { library->commitTypes(); }).fatal("Failed to commit types");

    // Get tables.
    sql::ext::TypedTable<int64_t, std::vector<float>>   fooTable(library->getDatabase().getTable(fooType.getName()));
    sql::ext::TypedTable<int64_t, std::vector<int32_t>> barTable(library->getDatabase().getTable(barType.getName()));
    sql::ext::TypedTable<int64_t, std::vector<uint64_t>, std::vector<double>> bazTable(
      library->getDatabase().getTable(bazType.getName()));

    // Create object handlers.
    auto fooHandler =
      library->createObjectHandler<alex::Member<&Foo::id>, alex::Member<&Foo::floats>>(fooType.getName());
    auto barHandler = library->createObjectHandler<alex::Member<&Bar::id>, alex::Member<&Bar::ints>>(barType.getName());
    auto bazHandler =
      library->createObjectHandler<alex::Member<&Baz::id>, alex::Member<&Baz::ints>, alex::Member<&Baz::floats>>(
        bazType.getName());

    // Insert Foo.
    {
        // Create objects.
        Foo foo0;
        foo0.floats.get().push_back(0.5f);
        foo0.floats.get().push_back(1.5f);
        Foo foo1;
        foo1.floats.get().push_back(-2.5f);
        foo1.floats.get().push_back(-3.5f);
        foo1.floats.get().push_back(-4.5f);

        // Try to insert.
        expectNoThrow([&] { fooHandler->insert(foo0); }).fatal("Failed to insert object");
        expectNoThrow([&] { fooHandler->insert(foo1); }).fatal("Failed to insert object");

        // Check assigned IDs.
        compareEQ(foo0.id, alex::InstanceId(1));
        compareEQ(foo1.id, alex::InstanceId(2));

        // Select inserted object using sql.
        Foo foo0_get = fooTable.selectOne<Foo>(fooTable.col<0>() == foo0.id.get(), true)(false);
        Foo foo1_get = fooTable.selectOne<Foo>(fooTable.col<0>() == foo1.id.get(), true)(false);

        // Compare objects.
        compareEQ(foo0.id, foo0_get.id);
        compareEQ(foo0.floats.get(), foo0_get.floats.get());
        compareEQ(foo1.id, foo1_get.id);
        compareEQ(foo1.floats.get(), foo1_get.floats.get());
    }

    // Insert Bar.
    {
        // Create objects.
        Bar bar0;
        bar0.ints.get().push_back(10);
        bar0.ints.get().push_back(100);
        Bar bar1;
        bar1.ints.get().push_back(-111);
        bar1.ints.get().push_back(-2222);
        bar1.ints.get().push_back(-33333);

        // Try to insert.
        expectNoThrow([&] { barHandler->insert(bar0); }).fatal("Failed to insert object");
        expectNoThrow([&] { barHandler->insert(bar1); }).fatal("Failed to insert object");
        // Check assigned IDs.
        compareEQ(bar0.id, alex::InstanceId(1));
        compareEQ(bar1.id, alex::InstanceId(2));

        // Select inserted object using sql.
        Bar bar0_get = barTable.selectOne<Bar>(barTable.col<0>() == bar0.id.get(), true)(false);
        Bar bar1_get = barTable.selectOne<Bar>(barTable.col<0>() == bar1.id.get(), true)(false);

        // Compare objects.
        compareEQ(bar0.id, bar0_get.id);
        compareEQ(bar0.ints.get(), bar0_get.ints.get());
        compareEQ(bar1.id, bar1_get.id);
        compareEQ(bar1.ints.get(), bar1_get.ints.get());
    }

    // Insert Baz.
    {
        // Create objects.
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

        // Try to insert.
        expectNoThrow([&] { bazHandler->insert(baz0); }).fatal("Failed to insert object");
        expectNoThrow([&] { bazHandler->insert(baz1); }).fatal("Failed to insert object");
        // Check assigned IDs.
        compareEQ(baz0.id, alex::InstanceId(1));
        compareEQ(baz1.id, alex::InstanceId(2));

        // Select inserted object using sql.
        Baz baz0_get = bazTable.selectOne<Baz>(bazTable.col<0>() == baz0.id.get(), true)(false);
        Baz baz1_get = bazTable.selectOne<Baz>(bazTable.col<0>() == baz1.id.get(), true)(false);

        // Compare objects.
        compareEQ(baz0.id, baz0_get.id);
        compareEQ(baz0.ints.get(), baz0_get.ints.get());
        compareEQ(baz0.floats.get(), baz0_get.floats.get());
        compareEQ(baz1.id, baz1_get.id);
        compareEQ(baz1.ints.get(), baz1_get.ints.get());
        compareEQ(baz1.floats.get(), baz1_get.floats.get());
    }
}
