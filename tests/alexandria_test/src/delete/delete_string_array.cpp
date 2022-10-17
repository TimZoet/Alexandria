#include "alexandria_test/delete/delete_string_array.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria/library.h"
#include "alexandria/member_types/member.h"
#include "alexandria/member_types/string_array.h"

namespace
{
    struct Foo
    {
        alex::InstanceId  id;
        alex::StringArray strings;

        Foo() = default;

        Foo(alex::InstanceId iid, std::vector<std::string> sstrings) : id(iid) { strings.get() = std::move(sstrings); }
    };

    struct Bar
    {
        alex::InstanceId  id;
        alex::StringArray strings1;
        alex::StringArray strings2;

        Bar() = default;

        Bar(alex::InstanceId iid, std::vector<std::string> sstrings1, std::vector<std::string> sstrings2) : id(iid)
        {
            strings1.get() = std::move(sstrings1);
            strings2.get() = std::move(sstrings2);
        }
    };
}  // namespace

void DeleteStringArray::operator()()
{
    // Create type with 1 string.
    auto& fooType = library->createType("Foo");
    fooType.createStringArrayProperty("strings");

    // Create type with 2 strings.
    auto& barType = library->createType("Bar");
    barType.createStringArrayProperty("strings1");
    barType.createStringArrayProperty("strings2");

    // Commit types.
    expectNoThrow([this]() { library->commitTypes(); }).fatal("Failed to commit types");

    // Get tables.
    sql::TypedTable<int64_t>                       fooTable(library->getDatabase().getTable(fooType.getName()));
    sql::TypedTable<int64_t, int64_t, std::string> fooStringsTable(
      library->getDatabase().getTable(fooType.getName() + "_strings"));
    sql::TypedTable<int64_t>                       barTable(library->getDatabase().getTable(barType.getName()));
    sql::TypedTable<int64_t, int64_t, std::string> barStrings1Table(
      library->getDatabase().getTable(barType.getName() + "_strings1"));
    sql::TypedTable<int64_t, int64_t, std::string> barStrings2Table(
      library->getDatabase().getTable(barType.getName() + "_strings2"));

    // Create object handlers.
    auto fooHandler =
      library->createObjectHandler<alex::Member<&Foo::id>, alex::Member<&Foo::strings>>(fooType.getName());
    auto barHandler =
      library->createObjectHandler<alex::Member<&Bar::id>, alex::Member<&Bar::strings1>, alex::Member<&Bar::strings2>>(
        barType.getName());

    // Delete Foo.
    {
        // Create and insert objects.
        Foo foo0;
        foo0.strings.get().push_back("abc");
        foo0.strings.get().push_back("def");
        Foo foo1;
        foo1.strings.get().push_back("10");
        foo1.strings.get().push_back("1111");
        foo1.strings.get().push_back("%^&*&(*U");
        expectNoThrow([&] { fooHandler->insert(foo0); }).fatal("Failed to insert object");
        expectNoThrow([&] { fooHandler->insert(foo1); }).fatal("Failed to insert object");

        // Delete objects one by one and check tables.
        compareEQ(static_cast<size_t>(2), fooTable.countAll()(true));
        compareEQ(static_cast<size_t>(2 + 3), fooStringsTable.countAll()(true));
        expectNoThrow([&] { fooHandler->del(foo0.id); });
        compareEQ(static_cast<size_t>(1), fooTable.countAll()(true));
        compareEQ(static_cast<size_t>(3), fooStringsTable.countAll()(true));
        expectNoThrow([&] { fooHandler->del(foo1.id); });
        compareEQ(static_cast<size_t>(0), fooTable.countAll()(true));
        compareEQ(static_cast<size_t>(0), fooStringsTable.countAll()(true));
    }

    // Delete Bar.
    {
        // Create and insert objects.
        Bar bar0;
        Bar bar1;
        bar1.strings1.get().push_back("");
        bar1.strings1.get().push_back("");
        bar1.strings1.get().push_back("hntfdrgtef");
        bar1.strings2.get().push_back("dbsfdcesw");
        bar1.strings2.get().push_back("utikrt");
        bar1.strings2.get().push_back("hntfdrgtef");
        expectNoThrow([&] { barHandler->insert(bar0); }).fatal("Failed to insert object");
        expectNoThrow([&] { barHandler->insert(bar1); }).fatal("Failed to insert object");

        // Delete objects one by one and check tables.
        compareEQ(static_cast<size_t>(2), barTable.countAll()(true));
        compareEQ(static_cast<size_t>(0 + 3), barStrings1Table.countAll()(true));
        compareEQ(static_cast<size_t>(0 + 3), barStrings2Table.countAll()(true));
        expectNoThrow([&] { barHandler->del(bar0.id); });
        compareEQ(static_cast<size_t>(1), barTable.countAll()(true));
        compareEQ(static_cast<size_t>(3), barStrings1Table.countAll()(true));
        compareEQ(static_cast<size_t>(3), barStrings2Table.countAll()(true));
        expectNoThrow([&] { barHandler->del(bar1.id); });
        compareEQ(static_cast<size_t>(0), barTable.countAll()(true));
        compareEQ(static_cast<size_t>(0), barStrings1Table.countAll()(true));
        compareEQ(static_cast<size_t>(0), barStrings2Table.countAll()(true));
    }
}
