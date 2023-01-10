#include "alexandria_test/insert/insert_primitive_blob.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria/core/library.h"
#include "alexandria/queries/insert_query.h"

namespace
{
    struct Foo
    {
        alex::InstanceId           id;
        alex::PrimitiveBlob<float> floats;

        Foo() = default;

        Foo(const alex::InstanceId iid, std::vector<float> ffloats) : id(iid) { floats.get() = std::move(ffloats); }
    };

    struct Bar
    {
        alex::InstanceId             id;
        alex::PrimitiveBlob<int32_t> ints;

        Bar() = default;

        Bar(const alex::InstanceId iid, std::vector<int32_t> iints) : id(iid) { ints.get() = std::move(iints); }
    };

    struct Baz
    {
        alex::InstanceId              id;
        alex::PrimitiveBlob<uint64_t> ints;
        alex::PrimitiveBlob<double>   floats;

        Baz() = default;

        Baz(const alex::InstanceId iid, std::vector<uint64_t> iints, std::vector<double> ffloats) : id(iid)
        {
            ints.get()   = std::move(iints);
            floats.get() = std::move(ffloats);
        }
    };

    using FooDescriptor = alex::GenerateTypeDescriptor<alex::Member<&Foo::id>, alex::Member<&Foo::floats>>;

    using BarDescriptor = alex::GenerateTypeDescriptor<alex::Member<&Bar::id>, alex::Member<&Bar::ints>>;

    using BazDescriptor =
      alex::GenerateTypeDescriptor<alex::Member<&Baz::id>, alex::Member<&Baz::ints>, alex::Member<&Baz::floats>>;
}  // namespace

void InsertPrimitiveBlob::operator()()
{
    // Create type with floats.
    auto& fooType = nameSpace->createType("Foo");
    fooType.createPrimitiveBlobProperty("floats", alex::DataType::Float);

    // Create type with integers.
    auto& barType = nameSpace->createType("Bar");
    barType.createPrimitiveBlobProperty("ints", alex::DataType::Int32);

    // Create type with floats and integers.
    auto& bazType = nameSpace->createType("Baz");
    bazType.createPrimitiveBlobProperty("uints", alex::DataType::Uint64);
    bazType.createPrimitiveBlobProperty("doubles", alex::DataType::Double);

    // Commit types.
    expectNoThrow([&] {
        fooType.commit();
        barType.commit();
        bazType.commit();
    }).fatal("Failed to commit types");

    // Insert Foo.
    {
        const sql::TypedTable<sql::row_id, std::string, std::vector<float>> table(
          library->getDatabase().getTable("main_Foo"));

        auto inserter = alex::InsertQuery(FooDescriptor(fooType));

        // Create objects.
        Foo foo0;
        foo0.floats.get().push_back(0.5f);
        foo0.floats.get().push_back(1.5f);
        Foo foo1;
        foo1.floats.get().push_back(-2.5f);
        foo1.floats.get().push_back(-3.5f);
        foo1.floats.get().push_back(-4.5f);

        // Try to insert.
        expectNoThrow([&] { inserter(foo0); }).fatal("Failed to insert object");
        expectNoThrow([&] { inserter(foo1); }).fatal("Failed to insert object");

        // Check assigned IDs.
        compareTrue(foo0.id.valid());
        compareTrue(foo1.id.valid());

        // Select inserted object using sql and compare.
        std::string id;
        auto        stmt    = table.selectAs<Foo, 1, 2>().where(like(table.col<1>(), &id)).compileOne();
        id                  = foo0.id.getAsString();
        const auto foo0_get = stmt.bind(sql::BindParameters::All)();
        id                  = foo1.id.getAsString();
        const auto foo1_get = stmt.bind(sql::BindParameters::All)();

        // Compare objects.
        compareEQ(foo0.id, foo0_get.id);
        compareEQ(foo0.floats.get(), foo0_get.floats.get());
        compareEQ(foo1.id, foo1_get.id);
        compareEQ(foo1.floats.get(), foo1_get.floats.get());
    }

    // Insert Bar.
    {
        const sql::TypedTable<sql::row_id, std::string, std::vector<int32_t>> table(
          library->getDatabase().getTable("main_Bar"));

        auto inserter = alex::InsertQuery(BarDescriptor(barType));

        // Create objects.
        Bar bar0;
        bar0.ints.get().push_back(10);
        bar0.ints.get().push_back(100);
        Bar bar1;
        bar1.ints.get().push_back(-111);
        bar1.ints.get().push_back(-2222);
        bar1.ints.get().push_back(-33333);

        // Try to insert.
        expectNoThrow([&] { inserter(bar0); }).fatal("Failed to insert object");
        expectNoThrow([&] { inserter(bar1); }).fatal("Failed to insert object");

        // Check assigned IDs.
        compareTrue(bar0.id.valid());
        compareTrue(bar1.id.valid());

        // Select inserted object using sql and compare.
        std::string id;
        auto        stmt    = table.selectAs<Bar, 1, 2>().where(like(table.col<1>(), &id)).compileOne();
        id                  = bar0.id.getAsString();
        const auto bar0_get = stmt.bind(sql::BindParameters::All)();
        id                  = bar1.id.getAsString();
        const auto bar1_get = stmt.bind(sql::BindParameters::All)();

        // Compare objects.
        compareEQ(bar0.id, bar0_get.id);
        compareEQ(bar0.ints.get(), bar0_get.ints.get());
        compareEQ(bar1.id, bar1_get.id);
        compareEQ(bar1.ints.get(), bar1_get.ints.get());
    }

    // Insert Baz.
    {
        const sql::TypedTable<sql::row_id, std::string, std::vector<uint64_t>, std::vector<double>> table(
          library->getDatabase().getTable("main_Baz"));

        auto inserter = alex::InsertQuery(BazDescriptor(bazType));

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
        expectNoThrow([&] { inserter(baz0); }).fatal("Failed to insert object");
        expectNoThrow([&] { inserter(baz1); }).fatal("Failed to insert object");

        // Check assigned IDs.
        compareTrue(baz0.id.valid());
        compareTrue(baz1.id.valid());

        // Select inserted object using sql and compare.
        std::string id;
        auto        stmt    = table.selectAs<Baz, 1, 2, 3>().where(like(table.col<1>(), &id)).compileOne();
        id                  = baz0.id.getAsString();
        const auto baz0_get = stmt.bind(sql::BindParameters::All)();
        id                  = baz1.id.getAsString();
        const auto baz1_get = stmt.bind(sql::BindParameters::All)();

        // Compare objects.
        compareEQ(baz0.id, baz0_get.id);
        compareEQ(baz0.ints.get(), baz0_get.ints.get());
        compareEQ(baz0.floats.get(), baz0_get.floats.get());
        compareEQ(baz1.id, baz1_get.id);
        compareEQ(baz1.ints.get(), baz1_get.ints.get());
        compareEQ(baz1.floats.get(), baz1_get.floats.get());
    }
}
