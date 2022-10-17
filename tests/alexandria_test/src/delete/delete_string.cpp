#include "alexandria_test/delete/delete_string.h"

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

void DeleteString::operator()()
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

    // Get tables.
    sql::TypedTable<int64_t, std::string> fooTable(library->getDatabase().getTable(fooType.getName()));
    sql::TypedTable<int64_t, std::string, std::string> barTable(
      library->getDatabase().getTable(barType.getName()));

    // Create object handlers.
    auto fooHandler = library->createObjectHandler<alex::Member<&Foo::id>, alex::Member<&Foo::a>>(fooType.getName());
    auto barHandler =
      library->createObjectHandler<alex::Member<&Bar::id>, alex::Member<&Bar::a>, alex::Member<&Bar::b>>(
        barType.getName());

    // Delete Foo.
    {
        // Create and insert objects.
        Foo foo0{.a = "abc"};
        Foo foo1{.a = "def"};
        expectNoThrow([&] { fooHandler->insert(foo0); }).fatal("Failed to insert object");
        expectNoThrow([&] { fooHandler->insert(foo1); }).fatal("Failed to insert object");

        // Delete objects one by one and check tables.
        compareEQ(static_cast<size_t>(2), fooTable.countAll()(true));
        expectNoThrow([&] { fooHandler->del(foo0.id); });
        compareEQ(static_cast<size_t>(1), fooTable.countAll()(true));
        expectNoThrow([&] { fooHandler->del(foo1.id); });
        compareEQ(static_cast<size_t>(0), fooTable.countAll()(true));
    }

    // Delete Bar.
    {
        // Create and insert objects.
        Bar bar0{.a = "hijkl", .b = "aaaa"};
        Bar bar1{.a = "^%*&", .b = ""};
        expectNoThrow([&] { barHandler->insert(bar0); }).fatal("Failed to insert object");
        expectNoThrow([&] { barHandler->insert(bar1); }).fatal("Failed to insert object");

        // Delete objects one by one and check tables.
        compareEQ(static_cast<size_t>(2), barTable.countAll()(true));
        expectNoThrow([&] { barHandler->del(bar0.id); });
        compareEQ(static_cast<size_t>(1), barTable.countAll()(true));
        expectNoThrow([&] { barHandler->del(bar1.id); });
        compareEQ(static_cast<size_t>(0), barTable.countAll()(true));
    }
}
