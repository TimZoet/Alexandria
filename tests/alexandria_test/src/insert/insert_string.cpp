#include "alexandria_test/insert/insert_string.h"

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

void InsertString::operator()()
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
    sql::ext::TypedTable<int64_t, std::string> fooTable(library->getDatabase().getTable(fooType.getName()));
    sql::ext::TypedTable<int64_t, std::string, std::string> barTable(
      library->getDatabase().getTable(barType.getName()));

    // Create object handlers.
    auto fooHandler = library->createObjectHandler<alex::Member<&Foo::id>, alex::Member<&Foo::a>>(fooType.getName());
    auto barHandler =
      library->createObjectHandler<alex::Member<&Bar::id>, alex::Member<&Bar::a>, alex::Member<&Bar::b>>(
        barType.getName());

    // Insert Foo.
    {
        // Create objects.
        Foo foo0{.a = "abc"};
        Foo foo1{.a = "def"};

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
        compareEQ(foo1.id, foo1_get.id);
        compareEQ(foo1.a, foo1_get.a);
    }

    // Insert Bar.
    {
        // Create objects.
        Bar bar0{.a = "hijkl", .b = "aaaa"};
        Bar bar1{.a = "^%*&", .b = ""};

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
        compareEQ(bar1.id, bar1_get.id);
        compareEQ(bar1.a, bar1_get.a);
        compareEQ(bar1.b, bar1_get.b);
    }
}
