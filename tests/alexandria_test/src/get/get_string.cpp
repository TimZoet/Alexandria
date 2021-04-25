#include "alexandria_test/get/get_string.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria/library.h"
#include "alexandria/member_types/member.h"

namespace
{
    struct Foo
    {
        alex::InstanceId id;
        std::string      a;
    };

    struct Bar
    {
        alex::InstanceId id;
        std::string      a;
        std::string      b;
    };
}  // namespace

void GetString::operator()()
{
    // Create type with 1 string.
    auto& fooType = library->createType("Foo");
    fooType.createStringProperty("prop1");

    // Create type with 2 strings.
    auto& barType = library->createType("Bar");
    barType.createStringProperty("prop1");
    barType.createStringProperty("prop2");

    // Commit types.
    expectNoThrow([this]() { library->commitTypes(); }).fatal("Failed to commit types");

    // Create object handlers.
    auto fooHandler = library->createObjectHandler<alex::Member<&Foo::id>, alex::Member<&Foo::a>>(fooType.getName());
    auto barHandler =
      library->createObjectHandler<alex::Member<&Bar::id>, alex::Member<&Bar::a>, alex::Member<&Bar::b>>(
        barType.getName());

    // Retrieve Foo.
    {
        // Create and insert objects.
        Foo foo0{.a = "abc"};
        Foo foo1{.a = "def"};
        expectNoThrow([&] { fooHandler.insert(foo0); }).fatal("Failed to insert object");
        expectNoThrow([&] { fooHandler.insert(foo1); }).fatal("Failed to insert object");

        // Try to retrieve objects.
        std::unique_ptr<Foo> foo0_get, foo1_get;
        expectNoThrow([&] { foo0_get = fooHandler.get(foo0.id); }).fatal("Failed to get object");
        expectNoThrow([&] { foo1_get = fooHandler.get(foo1.id); }).fatal("Failed to get object");

        // Compare objects.
        compareEQ(foo0.id, foo0_get->id);
        compareEQ(foo0.a, foo0_get->a);
        compareEQ(foo1.id, foo1_get->id);
        compareEQ(foo1.a, foo1_get->a);
    }

    // Retrieve Bar.
    {
        // Create and insert objects.
        Bar bar0{.a = "hijkl", .b = "aaaa"};
        Bar bar1{.a = "^%*&", .b = ""};
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
        compareEQ(bar1.id, bar1_get->id);
        compareEQ(bar1.a, bar1_get->a);
        compareEQ(bar1.b, bar1_get->b);
    }
}