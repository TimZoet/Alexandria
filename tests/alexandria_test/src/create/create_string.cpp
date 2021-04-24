#include "alexandria_test/create/create_string.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria/library.h"

namespace
{
    struct Foo
    {
        alex::InstanceId id;
        std::string      a;
        std::string      b;
    };
}  // namespace

void CreateString::operator()()
{
    // Create type with 2 strings.
    auto& fooType = library->createType("Foo");
    fooType.createStringProperty("prop0").setDefaultValue(std::string("abc"));
    fooType.createStringProperty("prop1").setDefaultValue(std::string("def"));

    // Commit types.
    expectNoThrow([this]() { library->commitTypes(); }).fatal("Failed to commit types");

    // Create object handlers.
    auto fooHandler = library->createObjectHandler<&Foo::id, &Foo::a, &Foo::b>(fooType.getName());

    // Insert Foo.
    {
        std::unique_ptr<Foo> foo0, foo1;

        // Try to create with default values.
        expectNoThrow([&] { foo0 = fooHandler.create(); }).fatal("Failed to create object");
        expectNoThrow([&] { foo1 = fooHandler.create(); }).fatal("Failed to create object");

        // Compare objects.
        compareEQ(foo0->id, alex::InstanceId(1));
        compareEQ(foo0->a, std::string("abc"));
        compareEQ(foo0->b, std::string("def"));
        compareEQ(foo1->id, alex::InstanceId(2));
        compareEQ(foo1->a, std::string("abc"));
        compareEQ(foo1->b, std::string("def"));
    }
}
