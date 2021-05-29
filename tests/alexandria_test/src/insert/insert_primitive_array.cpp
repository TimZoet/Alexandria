#include "alexandria_test/insert/insert_primitive_array.h"

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

void InsertPrimitiveArray::operator()()
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
        auto foo0_get = fooTable.selectOne(fooTable.col<0>() == foo0.id.get(), true)(false);
        auto foo1_get = fooTable.selectOne(fooTable.col<0>() == foo1.id.get(), true)(false);

        // Compare objects.
        compareEQ(foo0.id, std::get<0>(foo0_get));
        compareEQ(foo1.id, std::get<0>(foo1_get));

        // Select floats in separate table.
        auto               idparam       = foo0.id.get();
        auto               floats_select = fooFloatsTable.select<float, 2>(fooFloatsTable.col<1>() == &idparam, true);
        std::vector<float> floats_get(floats_select.begin(), floats_select.end());
        compareEQ(foo0.floats.get(), floats_get);
        idparam = foo1.id;
        floats_get.assign(floats_select(true).begin(), floats_select.end());
        compareEQ(foo1.floats.get(), floats_get);
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
        auto bar0_get = barTable.selectOne(barTable.col<0>() == bar0.id.get(), true)(false);
        auto bar1_get = barTable.selectOne(barTable.col<0>() == bar1.id.get(), true)(false);

        // Compare objects.
        compareEQ(bar0.id, std::get<0>(bar0_get));
        compareEQ(bar1.id, std::get<0>(bar1_get));

        // Select ints in separate table.
        auto                 idparam     = bar0.id.get();
        auto                 ints_select = barIntsTable.select<int32_t, 2>(barIntsTable.col<1>() == &idparam, true);
        std::vector<int32_t> ints_get(ints_select.begin(), ints_select.end());
        compareEQ(bar0.ints.get(), ints_get);
        idparam = bar1.id;
        ints_get.assign(ints_select(true).begin(), ints_select.end());
        compareEQ(bar1.ints.get(), ints_get);
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
        auto baz0_get = bazTable.selectOne(bazTable.col<0>() == baz0.id.get(), true)(false);
        auto baz1_get = bazTable.selectOne(bazTable.col<0>() == baz1.id.get(), true)(false);

        // Compare objects.
        compareEQ(baz0.id, std::get<0>(baz0_get));
        compareEQ(baz1.id, std::get<0>(baz1_get));

        // Select ints and floats in separate table.
        auto idparam        = baz0.id.get();
        auto uints_select   = bazIntsTable.select<uint64_t, 2>(bazIntsTable.col<1>() == &idparam, true);
        auto doubles_select = bazFloatsTable.select<float, 2>(bazFloatsTable.col<1>() == &idparam, true);
        std::vector<uint64_t> uints_get(uints_select.begin(), uints_select.end());
        std::vector<double>   doubles_get(doubles_select.begin(), doubles_select.end());
        compareEQ(baz0.ints.get(), uints_get);
        compareEQ(baz0.floats.get(), doubles_get);
        idparam = baz1.id;
        uints_get.assign(uints_select(true).begin(), uints_select.end());
        doubles_get.assign(doubles_select(true).begin(), doubles_select.end());
        compareEQ(baz1.ints.get(), uints_get);
        compareEQ(baz1.floats.get(), doubles_get);
    }
}
