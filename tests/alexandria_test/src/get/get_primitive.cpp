#include "alexandria_test/get/get_primitive.h"

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

void GetPrimitive::operator()()
{
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
    expectNoThrow([this]() { library->commitTypes(); }).fatal("Failed to commit types");

    // Create object handlers.
    auto fooHandler = library->createObjectHandler<&Foo::id, &Foo::a, &Foo::b>(fooType.getName());
    auto barHandler = library->createObjectHandler<&Bar::id, &Bar::a, &Bar::b, &Bar::c, &Bar::d>(barType.getName());
    auto bazHandler = library->createObjectHandler<&Baz::id, &Baz::a>(bazType.getName());

    // Retrieve Foo.
    {
        // Create and insert objects.
        Foo foo0{.a = 0.5f, .b = 1.5};
        Foo foo1{.a = -0.5f, .b = -1.5};
        expectNoThrow([&] { fooHandler.insert(foo0); }).fatal("Failed to insert object");
        expectNoThrow([&] { fooHandler.insert(foo1); }).fatal("Failed to insert object");

        // Try to retrieve objects.
        std::unique_ptr<Foo> foo0_get, foo1_get;
        expectNoThrow([&] { foo0_get = fooHandler.get(foo0.id); }).fatal("Failed to get object");
        expectNoThrow([&] { foo1_get = fooHandler.get(foo1.id); }).fatal("Failed to get object");

        // Compare objects.
        compareEQ(foo0.id, foo0_get->id);
        compareEQ(foo0.a, foo0_get->a);
        compareEQ(foo0.b, foo0_get->b);
        compareEQ(foo1.id, foo1_get->id);
        compareEQ(foo1.a, foo1_get->a);
        compareEQ(foo1.b, foo1_get->b);
    }

    // Retrieve Bar.
    {
        // Create and insert objects.
        Bar bar0{.a = 1, .b = 2, .c = 3, .d = 4};
        Bar bar1{.a = -1, .b = -2, .c = 123456, .d = 1234567};
        expectNoThrow([&] { barHandler.insert(bar0); }).fatal("Failed to insert object");
        expectNoThrow([&] { barHandler.insert(bar1); }).fatal("Failed to insert object");

        // Try to retrieve objects.
        std::unique_ptr<Bar> bar0_get, bar1_get;
        expectNoThrow([&] { bar0_get = barHandler.get(bar0.id); }).fatal("Failed to get object");
        expectNoThrow([&] { bar1_get = barHandler.get(bar1.id); }).fatal("Failed to get object");

        // Compare objects.
        compareEQ(bar0.id, bar0_get->id);
        compareEQ(bar0.a, bar0_get->a);
        compareEQ(bar0.b, bar0_get->b);
        compareEQ(bar0.c, bar0_get->c);
        compareEQ(bar0.d, bar0_get->d);
        compareEQ(bar1.id, bar1_get->id);
        compareEQ(bar1.a, bar1_get->a);
        compareEQ(bar1.b, bar1_get->b);
        compareEQ(bar1.c, bar1_get->c);
        compareEQ(bar1.d, bar1_get->d);
    }

    // Retrieve Baz.
    {
        // Create and insert objects.
        Baz baz0{.a = "abc"};
        Baz baz1{.a = "defghi"};
        Baz baz2{.a = ""};
        expectNoThrow([&] { bazHandler.insert(baz0); }).fatal("Failed to insert object");
        expectNoThrow([&] { bazHandler.insert(baz1); }).fatal("Failed to insert object");
        expectNoThrow([&] { bazHandler.insert(baz2); }).fatal("Failed to insert object");

        // Try to retrieve objects.
        std::unique_ptr<Baz> baz0_get, baz1_get, baz2_get;
        expectNoThrow([&] { baz0_get = bazHandler.get(baz0.id); }).fatal("Failed to get object");
        expectNoThrow([&] { baz1_get = bazHandler.get(baz1.id); }).fatal("Failed to get object");
        expectNoThrow([&] { baz2_get = bazHandler.get(baz2.id); }).fatal("Failed to get object");

        // Compare objects.
        compareEQ(baz0.id, baz0_get->id);
        compareEQ(baz0.a, baz0_get->a);
        compareEQ(baz1.id, baz1_get->id);
        compareEQ(baz1.a, baz1_get->a);
        compareEQ(baz2.id, baz2_get->id);
        compareEQ(baz2.a, baz2_get->a);
    }
}