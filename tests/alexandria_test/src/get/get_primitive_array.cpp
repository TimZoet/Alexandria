#include "alexandria_test/get/get_primitive_array.h"

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

void GetPrimitiveArray::operator()()
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
    auto fooHandler = library->createObjectHandler<&Foo::id, &Foo::floats>(fooType.getName());
    auto barHandler = library->createObjectHandler<&Bar::id, &Bar::ints>(barType.getName());
    auto bazHandler = library->createObjectHandler<&Baz::id, &Baz::ints, &Baz::floats>(bazType.getName());

    // Retrieve Foo.
    {
        // Create and insert objects.
        Foo foo0;
        foo0.floats.getPrimitiveArray().push_back(0.5f);
        foo0.floats.getPrimitiveArray().push_back(1.5f);
        Foo foo1;
        foo1.floats.getPrimitiveArray().push_back(-2.5f);
        foo1.floats.getPrimitiveArray().push_back(-3.5f);
        foo1.floats.getPrimitiveArray().push_back(-4.5f);
        expectNoThrow([&] { fooHandler.insert(foo0); }).fatal("Failed to insert object");
        expectNoThrow([&] { fooHandler.insert(foo1); }).fatal("Failed to insert object");

        // Try to retrieve objects.
        std::unique_ptr<Foo> foo0_get, foo1_get;
        expectNoThrow([&] { foo0_get = fooHandler.get(foo0.id); }).fatal("Failed to get object");
        expectNoThrow([&] { foo1_get = fooHandler.get(foo1.id); }).fatal("Failed to get object");

        // Compare objects.
        compareEQ(foo0.id, foo0_get->id);
        compareEQ(foo0.floats.getPrimitiveArray(), foo0_get->floats.getPrimitiveArray());
        compareEQ(foo1.id, foo1_get->id);
        compareEQ(foo1.floats.getPrimitiveArray(), foo1_get->floats.getPrimitiveArray());
    }

    // Retrieve Bar.
    {
        // Create and insert objects.
        Bar bar0;
        bar0.ints.getPrimitiveArray().push_back(10);
        bar0.ints.getPrimitiveArray().push_back(100);
        Bar bar1;
        bar1.ints.getPrimitiveArray().push_back(-111);
        bar1.ints.getPrimitiveArray().push_back(-2222);
        bar1.ints.getPrimitiveArray().push_back(-33333);
        expectNoThrow([&] { barHandler.insert(bar0); }).fatal("Failed to insert object");
        expectNoThrow([&] { barHandler.insert(bar1); }).fatal("Failed to insert object");

        // Try to retrieve objects.
        std::unique_ptr<Bar> bar0_get, bar1_get;
        expectNoThrow([&] { bar0_get = barHandler.get(bar0.id); }).fatal("Failed to get object");
        expectNoThrow([&] { bar1_get = barHandler.get(bar1.id); }).fatal("Failed to get object");

        // Compare objects.
        compareEQ(bar0.id, bar0_get->id);
        compareEQ(bar0.ints.getPrimitiveArray(), bar0_get->ints.getPrimitiveArray());
        compareEQ(bar1.id, bar1_get->id);
        compareEQ(bar1.ints.getPrimitiveArray(), bar1_get->ints.getPrimitiveArray());
    }

    // Retrieve Baz.
    {
        // Create and insert objects.
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
        expectNoThrow([&] { bazHandler.insert(baz0); }).fatal("Failed to insert object");
        expectNoThrow([&] { bazHandler.insert(baz1); }).fatal("Failed to insert object");

        // Try to retrieve objects.
        std::unique_ptr<Baz> baz0_get, baz1_get;
        expectNoThrow([&] { baz0_get = bazHandler.get(baz0.id); }).fatal("Failed to get object");
        expectNoThrow([&] { baz1_get = bazHandler.get(baz1.id); }).fatal("Failed to get object");

        // Compare objects.
        compareEQ(baz0.id, baz0_get->id);
        compareEQ(baz0.ints.getPrimitiveArray(), baz0_get->ints.getPrimitiveArray());
        compareEQ(baz0.floats.getPrimitiveArray(), baz0_get->floats.getPrimitiveArray());
        compareEQ(baz1.id, baz1_get->id);
        compareEQ(baz1.ints.getPrimitiveArray(), baz1_get->ints.getPrimitiveArray());
        compareEQ(baz1.floats.getPrimitiveArray(), baz1_get->floats.getPrimitiveArray());
    }
}
