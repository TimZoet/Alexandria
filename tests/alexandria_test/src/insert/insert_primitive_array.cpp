#include "alexandria_test/insert/insert_primitive_array.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria/library.h"
#include "alexandria/member_types/primitive_array.h"

namespace
{
    struct Foo
    {
        int64_t                     id = 0;
        alex::PrimitiveArray<float> floats;

        Foo() = default;

        Foo(int64_t iid, std::vector<float> ffloats) : id(iid) { floats.getPrimitiveArray() = std::move(ffloats); }
    };

    struct Bar
    {
        int64_t                       id = 0;
        alex::PrimitiveArray<int32_t> ints;

        Bar() = default;

        Bar(int64_t iid, std::vector<int32_t> iints) : id(iid) { ints.getPrimitiveArray() = std::move(iints); }
    };

    struct Baz
    {
        int64_t                        id = 0;
        alex::PrimitiveArray<uint64_t> ints;
        alex::PrimitiveArray<double>   floats;

        Baz() = default;

        Baz(int64_t iid, std::vector<uint64_t> iints, std::vector<double> ffloats) : id(iid)
        {
            ints.getPrimitiveArray()   = std::move(iints);
            floats.getPrimitiveArray() = std::move(ffloats);
        }
    };
}  // namespace

void InsertPrimitiveArray::operator()()
{
    // Create all property types.
    auto& floatsProp  = library->createPrimitiveArrayProperty("floats", alex::DataType::Float, false);
    auto& doublesProp = library->createPrimitiveArrayProperty("doubles", alex::DataType::Double, false);
    auto& intsProp    = library->createPrimitiveArrayProperty("ints", alex::DataType::Int32, false);
    auto& uintsProp   = library->createPrimitiveArrayProperty("uints", alex::DataType::Uint64, false);

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
    auto fooHandler = library->createObjectHandler<&Foo::id, &Foo::floats>(fooType.getName());
    auto barHandler = library->createObjectHandler<&Bar::id, &Bar::ints>(barType.getName());
    auto bazHandler = library->createObjectHandler<&Baz::id, &Baz::ints, &Baz::floats>(bazType.getName());

    // TODO: Use vector comparison methods once supported.

    // Insert Foo.
    Foo foo0;
    foo0.floats.getPrimitiveArray().push_back(0.5f);
    foo0.floats.getPrimitiveArray().push_back(1.5f);
    Foo foo1;
    foo1.floats.getPrimitiveArray().push_back(-2.5f);
    foo1.floats.getPrimitiveArray().push_back(-3.5f);
    foo1.floats.getPrimitiveArray().push_back(-4.5f);
    fooHandler.insert(foo0);
    fooHandler.insert(foo1);
    // Check assigned IDs.
    compareEQ(foo0.id, static_cast<int64_t>(1));
    compareEQ(foo1.id, static_cast<int64_t>(2));
    // Select inserted object using sql and compare.
    auto foo0_get = fooTable.selectOne(fooTable.col<0>() == foo0.id, true)(false);
    auto foo1_get = fooTable.selectOne(fooTable.col<0>() == foo1.id, true)(false);
    compareEQ(foo0.id, std::get<0>(foo0_get));
    compareEQ(foo1.id, std::get<0>(foo1_get));
    // Select floats in separate table.
    auto                           idparam       = foo0.id;
    auto                           floats_select = fooFloatsTable.select<2>(fooFloatsTable.col<1>() == &idparam, true);
    std::vector<std::tuple<float>> floats_get(floats_select.begin(), floats_select.end());
    compareEQ(foo0.floats.getPrimitiveArray()[0], std::get<0>(floats_get[0]));
    compareEQ(foo0.floats.getPrimitiveArray()[1], std::get<0>(floats_get[1]));
    idparam = foo1.id;
    floats_get.assign(floats_select(true).begin(), floats_select.end());
    compareEQ(foo1.floats.getPrimitiveArray()[0], std::get<0>(floats_get[0]));
    compareEQ(foo1.floats.getPrimitiveArray()[1], std::get<0>(floats_get[1]));
    compareEQ(foo1.floats.getPrimitiveArray()[2], std::get<0>(floats_get[2]));

    // Insert Bar.
    Bar bar0;
    bar0.ints.getPrimitiveArray().push_back(10);
    bar0.ints.getPrimitiveArray().push_back(100);
    Bar bar1;
    bar1.ints.getPrimitiveArray().push_back(-111);
    bar1.ints.getPrimitiveArray().push_back(-2222);
    bar1.ints.getPrimitiveArray().push_back(-33333);
    barHandler.insert(bar0);
    barHandler.insert(bar1);
    // Check assigned IDs.
    compareEQ(bar0.id, static_cast<int64_t>(1));
    compareEQ(bar1.id, static_cast<int64_t>(2));
    // Select inserted object using sql and compare.
    auto bar0_get = barTable.selectOne(barTable.col<0>() == bar0.id, true)(false);
    auto bar1_get = barTable.selectOne(barTable.col<0>() == bar1.id, true)(false);
    compareEQ(bar0.id, std::get<0>(bar0_get));
    compareEQ(bar1.id, std::get<0>(bar1_get));
    // Select ints in separate table.
    idparam                                      = bar0.id;
    auto                             ints_select = barIntsTable.select<2>(barIntsTable.col<1>() == &idparam, true);
    std::vector<std::tuple<int32_t>> ints_get(ints_select.begin(), ints_select.end());
    compareEQ(bar0.ints.getPrimitiveArray()[0], std::get<0>(ints_get[0]));
    compareEQ(bar0.ints.getPrimitiveArray()[1], std::get<0>(ints_get[1]));
    idparam = bar1.id;
    ints_get.assign(ints_select(true).begin(), ints_select.end());
    compareEQ(bar1.ints.getPrimitiveArray()[0], std::get<0>(ints_get[0]));
    compareEQ(bar1.ints.getPrimitiveArray()[1], std::get<0>(ints_get[1]));
    compareEQ(bar1.ints.getPrimitiveArray()[2], std::get<0>(ints_get[2]));

    // Insert Baz.
    Baz baz0;
    baz0.ints.getPrimitiveArray().push_back(10);
    baz0.ints.getPrimitiveArray().push_back(100);
    baz0.floats.getPrimitiveArray().push_back(0.5);
    baz0.floats.getPrimitiveArray().push_back(1.5);
    Baz baz1;
    baz1.ints.getPrimitiveArray().push_back(-111);
    baz1.ints.getPrimitiveArray().push_back(-2222);
    baz1.ints.getPrimitiveArray().push_back(-33333);
    baz1.floats.getPrimitiveArray().push_back(-2.5);
    baz1.floats.getPrimitiveArray().push_back(-3.5);
    baz1.floats.getPrimitiveArray().push_back(-4.5);
    bazHandler.insert(baz0);
    bazHandler.insert(baz1);
    // Check assigned IDs.
    compareEQ(baz0.id, static_cast<int64_t>(1));
    compareEQ(baz1.id, static_cast<int64_t>(2));
    // Select inserted object using sql and compare.
    auto baz0_get = bazTable.selectOne(bazTable.col<0>() == baz0.id, true)(false);
    auto baz1_get = bazTable.selectOne(bazTable.col<0>() == baz1.id, true)(false);
    compareEQ(baz0.id, std::get<0>(baz0_get));
    compareEQ(baz1.id, std::get<0>(baz1_get));
    // Select ints and floats in separate table.
    idparam                                        = baz0.id;
    auto                              uints_select = bazIntsTable.select<2>(bazIntsTable.col<1>() == &idparam, true);
    std::vector<std::tuple<uint64_t>> uints_get(uints_select.begin(), uints_select.end());
    compareEQ(baz0.ints.getPrimitiveArray()[0], std::get<0>(uints_get[0]));
    compareEQ(baz0.ints.getPrimitiveArray()[1], std::get<0>(uints_get[1]));
    auto doubles_select = bazFloatsTable.select<2>(bazFloatsTable.col<1>() == &idparam, true);
    std::vector<std::tuple<double>> doubles_get(doubles_select.begin(), doubles_select.end());
    compareEQ(baz0.floats.getPrimitiveArray()[0], std::get<0>(doubles_get[0]));
    compareEQ(baz0.floats.getPrimitiveArray()[1], std::get<0>(doubles_get[1]));
    idparam = baz1.id;
    doubles_get.assign(doubles_select(true).begin(), doubles_select.end());
    compareEQ(baz1.floats.getPrimitiveArray()[0], std::get<0>(doubles_get[0]));
    compareEQ(baz1.floats.getPrimitiveArray()[1], std::get<0>(doubles_get[1]));
    compareEQ(baz1.floats.getPrimitiveArray()[2], std::get<0>(doubles_get[2]));
}
