#include "alexandria_test/insert/insert_primitive_blob.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria/library.h"
#include "alexandria/member_types/primitive_blob.h"

namespace
{
    struct Foo
    {
        int64_t                    id = 0;
        alex::PrimitiveBlob<float> floats;

        Foo() = default;

        Foo(int64_t iid, std::vector<float> ffloats) : id(iid) { floats.get() = std::move(ffloats); }
    };

    struct Bar
    {
        int64_t                      id = 0;
        alex::PrimitiveBlob<int32_t> ints;

        Bar() = default;

        Bar(int64_t iid, std::vector<int32_t> iints) : id(iid) { ints.get() = std::move(iints); }
    };

    struct Baz
    {
        int64_t                       id = 0;
        alex::PrimitiveBlob<uint64_t> ints;
        alex::PrimitiveBlob<double>   floats;

        Baz() = default;

        Baz(int64_t iid, std::vector<uint64_t> iints, std::vector<double> ffloats) : id(iid)
        {
            ints.get()   = std::move(iints);
            floats.get() = std::move(ffloats);
        }
    };
}  // namespace

void InsertPrimitiveBlob::operator()()
{
    // Create all property types.
    auto& floatsProp  = library->createPrimitiveArrayProperty("floats", alex::DataType::Float, true);
    auto& doublesProp = library->createPrimitiveArrayProperty("doubles", alex::DataType::Double, true);
    auto& intsProp    = library->createPrimitiveArrayProperty("ints", alex::DataType::Int32, true);
    auto& uintsProp   = library->createPrimitiveArrayProperty("uints", alex::DataType::Uint64, true);

    // Create type with floats.
    auto& fooType = library->createType("Foo");
    fooType.addProperty(floatsProp);

    // Create type with integers.
    auto& barType = library->createType("Bar");
    barType.addProperty(intsProp);

    // Create type with floats and integers.
    auto& bazType = library->createType("Baz");
    bazType.addProperty(uintsProp);
    bazType.addProperty(doublesProp);

    // Commit types.
    expectNoThrow([this]() { library->commitTypes(); });

    // Get tables.
    sql::ext::TypedTable<int64_t, std::vector<float>>   fooTable(library->getDatabase().getTable(fooType.getName()));
    sql::ext::TypedTable<int64_t, std::vector<int32_t>> barTable(library->getDatabase().getTable(barType.getName()));
    sql::ext::TypedTable<int64_t, std::vector<uint64_t>, std::vector<double>> bazTable(
      library->getDatabase().getTable(bazType.getName()));

    // Create object handlers.
    auto fooHandler = library->createObjectHandler<&Foo::id, &Foo::floats>(fooType.getName());
    auto barHandler = library->createObjectHandler<&Bar::id, &Bar::ints>(barType.getName());
    auto bazHandler = library->createObjectHandler<&Baz::id, &Baz::ints, &Baz::floats>(bazType.getName());

    // TODO: Use vector comparison methods once supported.

    // Insert Foo.
    Foo foo0;
    foo0.floats.get().push_back(0.5f);
    foo0.floats.get().push_back(1.5f);
    Foo foo1;
    foo1.floats.get().push_back(-2.5f);
    foo1.floats.get().push_back(-3.5f);
    foo1.floats.get().push_back(-4.5f);
    fooHandler.insert(foo0);
    fooHandler.insert(foo1);
    // Check assigned IDs.
    compareEQ(foo0.id, static_cast<int64_t>(1));
    compareEQ(foo1.id, static_cast<int64_t>(2));
    // Select inserted object using sql and compare.
    Foo foo0_get = fooTable.selectOne<Foo>(fooTable.col<0>() == foo0.id, true)(false);
    Foo foo1_get = fooTable.selectOne<Foo>(fooTable.col<0>() == foo1.id, true)(false);
    compareEQ(foo0.id, foo0_get.id);
    compareEQ(foo0.floats.get().size(), foo0_get.floats.get().size());
    compareEQ(foo0.floats.get()[0], foo0_get.floats.get()[0]);
    compareEQ(foo0.floats.get()[1], foo0_get.floats.get()[1]);
    compareEQ(foo1.id, foo1_get.id);
    compareEQ(foo1.floats.get().size(), foo1_get.floats.get().size());
    compareEQ(foo1.floats.get()[0], foo1_get.floats.get()[0]);
    compareEQ(foo1.floats.get()[1], foo1_get.floats.get()[1]);
    compareEQ(foo1.floats.get()[2], foo1_get.floats.get()[2]);

    // Insert Bar.
    Bar bar0;
    bar0.ints.get().push_back(10);
    bar0.ints.get().push_back(100);
    Bar bar1;
    bar1.ints.get().push_back(-111);
    bar1.ints.get().push_back(-2222);
    bar1.ints.get().push_back(-33333);
    barHandler.insert(bar0);
    barHandler.insert(bar1);
    // Check assigned IDs.
    compareEQ(bar0.id, static_cast<int64_t>(1));
    compareEQ(bar1.id, static_cast<int64_t>(2));
    // Select inserted object using sql and compare.
    Bar bar0_get = barTable.selectOne<Bar>(barTable.col<0>() == bar0.id, true)(false);
    Bar bar1_get = barTable.selectOne<Bar>(barTable.col<0>() == bar1.id, true)(false);
    compareEQ(bar0.id, bar0_get.id);
    compareEQ(bar0.ints.get().size(), bar0_get.ints.get().size());
    compareEQ(bar0.ints.get()[0], bar0_get.ints.get()[0]);
    compareEQ(bar0.ints.get()[1], bar0_get.ints.get()[1]);
    compareEQ(bar1.id, bar1_get.id);
    compareEQ(bar1.ints.get().size(), bar1_get.ints.get().size());
    compareEQ(bar1.ints.get()[0], bar1_get.ints.get()[0]);
    compareEQ(bar1.ints.get()[1], bar1_get.ints.get()[1]);
    compareEQ(bar1.ints.get()[2], bar1_get.ints.get()[2]);

    // Insert Baz.
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
    bazHandler.insert(baz0);
    bazHandler.insert(baz1);
    // Check assigned IDs.
    compareEQ(baz0.id, static_cast<int64_t>(1));
    compareEQ(baz1.id, static_cast<int64_t>(2));
    // Select inserted object using sql and compare.
    Baz baz0_get = bazTable.selectOne<Baz>(bazTable.col<0>() == baz0.id, true)(false);
    Baz baz1_get = bazTable.selectOne<Baz>(bazTable.col<0>() == baz1.id, true)(false);
    compareEQ(baz0.id, baz0_get.id);
    compareEQ(baz0.ints.get().size(), baz0_get.ints.get().size());
    compareEQ(baz0.ints.get()[0], baz0_get.ints.get()[0]);
    compareEQ(baz0.ints.get()[1], baz0_get.ints.get()[1]);
    compareEQ(baz0.floats.get().size(), baz0_get.floats.get().size());
    compareEQ(baz0.floats.get()[0], baz0_get.floats.get()[0]);
    compareEQ(baz0.floats.get()[1], baz0_get.floats.get()[1]);
    compareEQ(baz1.id, baz1_get.id);
    compareEQ(baz1.ints.get().size(), baz1_get.ints.get().size());
    compareEQ(baz1.ints.get()[0], baz1_get.ints.get()[0]);
    compareEQ(baz1.ints.get()[1], baz1_get.ints.get()[1]);
    compareEQ(baz1.ints.get()[2], baz1_get.ints.get()[2]);
    compareEQ(baz1.floats.get().size(), baz1_get.floats.get().size());
    compareEQ(baz1.floats.get()[0], baz1_get.floats.get()[0]);
    compareEQ(baz1.floats.get()[1], baz1_get.floats.get()[1]);
    compareEQ(baz1.floats.get()[2], baz1_get.floats.get()[2]);
}
