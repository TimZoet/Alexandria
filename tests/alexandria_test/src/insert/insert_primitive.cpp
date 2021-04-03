#include "alexandria_test/insert/insert_primitive.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria/library.h"

namespace
{
    struct Foo
    {
        int64_t id = 0;
        float   a  = 0;
        double  b  = 0;
    };

    struct Bar
    {
        int64_t  id = 0;
        int32_t  a  = 0;
        int64_t  b  = 0;
        uint32_t c  = 0;
        uint64_t d  = 0;
    };

    struct Baz
    {
        int64_t     id = 0;
        std::string a;
    };
}  // namespace

void InsertPrimitive::operator()()
{
    // TODO: Here, or somewhere else, insert object with non-zero ID.
    
    // Create all property types.
    auto& floatProp  = library->createPrimitiveProperty("floatProp", alex::DataType::Float);
    auto& doubleProp = library->createPrimitiveProperty("doubleProp", alex::DataType::Double);
    auto& int32Prop  = library->createPrimitiveProperty("int32Prop", alex::DataType::Int32);
    auto& int64Prop  = library->createPrimitiveProperty("int64Prop", alex::DataType::Int64);
    auto& uint32Prop = library->createPrimitiveProperty("uint32Prop", alex::DataType::Uint32);
    auto& uint64Prop = library->createPrimitiveProperty("uint64Prop", alex::DataType::Uint64);
    auto& stringProp = library->createPrimitiveProperty("stringProp", alex::DataType::String);

    // Create type with floats.
    auto& fooType = library->createType("Foo");
    fooType.addProperty(floatProp);
    fooType.addProperty(doubleProp);

    // Create type with integers.
    auto& barType = library->createType("Bar");
    barType.addProperty(int32Prop);
    barType.addProperty(int64Prop);
    barType.addProperty(uint32Prop);
    barType.addProperty(uint64Prop);

    // Create type with string.
    auto& bazType = library->createType("Baz");
    bazType.addProperty(stringProp);

    // Commit types.
    expectNoThrow([this]() { library->commitTypes(); });

    // Get tables.
    sql::ext::TypedTable<int64_t, float, double> fooTable(library->getDatabase().getTable(fooType.getName()));
    sql::ext::TypedTable<int64_t, int32_t, int64_t, uint32_t, uint64_t> barTable(
      library->getDatabase().getTable(barType.getName()));
    sql::ext::TypedTable<int64_t, std::string> bazTable(library->getDatabase().getTable(bazType.getName()));

    // Create object handlers.
    auto fooHandler = library->createObjectHandler<&Foo::id, &Foo::a, &Foo::b>(fooType.getName());
    auto barHandler = library->createObjectHandler<&Bar::id, &Bar::a, &Bar::b, &Bar::c, &Bar::d>(barType.getName());
    auto bazHandler = library->createObjectHandler<&Baz::id, &Baz::a>(bazType.getName());

    // Insert Foo.
    Foo foo0{.a = 0.5f, .b = 1.5};
    Foo foo1{.a = -0.5f, .b = -1.5};
    fooHandler.insert(foo0);
    fooHandler.insert(foo1);
    // Check assigned IDs.
    compareEQ(foo0.id, static_cast<int64_t>(1));
    compareEQ(foo1.id, static_cast<int64_t>(2));
    // Select inserted object using sql and compare.
    Foo foo0_get = fooTable.selectOne<Foo>(fooTable.col<0>() == foo0.id, true)(false);
    Foo foo1_get = fooTable.selectOne<Foo>(fooTable.col<0>() == foo1.id, true)(false);
    compareEQ(foo0.id, foo0_get.id);
    compareEQ(foo0.a, foo0_get.a);
    compareEQ(foo0.b, foo0_get.b);
    compareEQ(foo1.id, foo1_get.id);
    compareEQ(foo1.a, foo1_get.a);
    compareEQ(foo1.b, foo1_get.b);

    // Insert Bar.
    Bar bar0{.a = 1, .b = 2, .c = 3, .d = 4};
    Bar bar1{.a = -1, .b = -2, .c = 123456, .d = 1234567};
    barHandler.insert(bar0);
    barHandler.insert(bar1);
    // Check assigned IDs.
    compareEQ(bar0.id, static_cast<int64_t>(1));
    compareEQ(bar1.id, static_cast<int64_t>(2));
    // Select inserted object using sql and compare.
    Bar bar0_get = barTable.selectOne<Bar>(barTable.col<0>() == bar0.id, true)(false);
    Bar bar1_get = barTable.selectOne<Bar>(barTable.col<0>() == bar1.id, true)(false);
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
