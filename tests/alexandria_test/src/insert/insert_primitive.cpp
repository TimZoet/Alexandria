#include "alexandria_test/insert/insert_primitive.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria/library.h"

namespace
{
    struct Foo
    {
        alex::InstanceId id;
        float            a = 0;
        double           b = 0;
    };

    struct Bar
    {
        alex::InstanceId id;
        int32_t          a = 0;
        int64_t          b = 0;
        uint32_t         c = 0;
        uint64_t         d = 0;
    };
}  // namespace

void InsertPrimitive::operator()()
{
    // TODO: Here, or somewhere else, insert object with non-zero ID.
    // Create type with floats.
    auto& fooType = library->createType("Foo");
    fooType.createPrimitiveProperty("floatProp", alex::DataType::Float);
    fooType.createPrimitiveProperty("doubleProp", alex::DataType::Double);

    // Create type with integers.
    auto& barType = library->createType("Bar");
    barType.createPrimitiveProperty("int32Prop", alex::DataType::Int32);
    barType.createPrimitiveProperty("int64Prop", alex::DataType::Int64);
    barType.createPrimitiveProperty("uint32Prop", alex::DataType::Uint32);
    barType.createPrimitiveProperty("uint64Prop", alex::DataType::Uint64);

    // Commit types.
    expectNoThrow([this]() { library->commitTypes(); }).fatal("Failed to commit types");

    // Get tables.
    sql::ext::TypedTable<int64_t, float, double> fooTable(library->getDatabase().getTable(fooType.getName()));
    sql::ext::TypedTable<int64_t, int32_t, int64_t, uint32_t, uint64_t> barTable(
      library->getDatabase().getTable(barType.getName()));

    // Create object handlers.
    auto fooHandler = library->createObjectHandler<&Foo::id, &Foo::a, &Foo::b>(fooType.getName());
    auto barHandler = library->createObjectHandler<&Bar::id, &Bar::a, &Bar::b, &Bar::c, &Bar::d>(barType.getName());

    // Insert Foo.
    {
        // Create objects.
        Foo foo0{.a = 0.5f, .b = 1.5};
        Foo foo1{.a = -0.5f, .b = -1.5};

        // Try to insert.
        expectNoThrow([&] { fooHandler.insert(foo0); }).fatal("Failed to insert object");
        expectNoThrow([&] { fooHandler.insert(foo1); }).fatal("Failed to insert object");

        // Check assigned IDs.
        compareEQ(foo0.id, alex::InstanceId(1));
        compareEQ(foo1.id, alex::InstanceId(2));

        // Select inserted object using sql and compare.
        Foo foo0_get = fooTable.selectOne<Foo>(fooTable.col<0>() == foo0.id.get(), true)(false);
        Foo foo1_get = fooTable.selectOne<Foo>(fooTable.col<0>() == foo1.id.get(), true)(false);

        // Compare objects.
        compareEQ(foo0.id, foo0_get.id);
        compareEQ(foo0.a, foo0_get.a);
        compareEQ(foo0.b, foo0_get.b);
        compareEQ(foo1.id, foo1_get.id);
        compareEQ(foo1.a, foo1_get.a);
        compareEQ(foo1.b, foo1_get.b);
    }

    // Insert Bar.
    {
        // Create objects.
        Bar bar0{.a = 1, .b = 2, .c = 3, .d = 4};
        Bar bar1{.a = -1, .b = -2, .c = 123456, .d = 1234567};

        // Try to insert.
        expectNoThrow([&] { barHandler.insert(bar0); }).fatal("Failed to insert object");
        expectNoThrow([&] { barHandler.insert(bar1); }).fatal("Failed to insert object");

        // Check assigned IDs.
        compareEQ(bar0.id, alex::InstanceId(1));
        compareEQ(bar1.id, alex::InstanceId(2));

        // Select inserted object using sql and compare.
        Bar bar0_get = barTable.selectOne<Bar>(barTable.col<0>() == bar0.id.get(), true)(false);
        Bar bar1_get = barTable.selectOne<Bar>(barTable.col<0>() == bar1.id.get(), true)(false);

        // Compare objects.
        compareEQ(bar0.id, bar0_get.id);
        compareEQ(bar0.a, bar0_get.a);
        compareEQ(bar0.b, bar0_get.b);
        compareEQ(bar0.c, bar0_get.c);
        compareEQ(bar0.d, bar0_get.d);
        compareEQ(bar1.id, bar1_get.id);
        compareEQ(bar1.a, bar1_get.a);
        compareEQ(bar1.b, bar1_get.b);
        compareEQ(bar1.c, bar1_get.c);
        compareEQ(bar1.d, bar1_get.d);
    }
}
