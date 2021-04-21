#include "alexandria_test/insert/insert_string_array.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria/library.h"
#include "alexandria/member_types/string_array.h"

namespace
{
    struct Foo
    {
        int64_t           id = 0;
        alex::StringArray strings;

        Foo() = default;

        Foo(int64_t iid, std::vector<std::string> sstrings) : id(iid) { strings.get() = std::move(sstrings); }
    };

    struct Bar
    {
        int64_t           id = 0;
        alex::StringArray strings1;
        alex::StringArray strings2;

        Bar() = default;

        Bar(int64_t iid, std::vector<std::string> sstrings1, std::vector<std::string> sstrings2) : id(iid)
        {
            strings1.get() = std::move(sstrings1);
            strings2.get() = std::move(sstrings2);
        }
    };
}  // namespace

void InsertStringArray::operator()()
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
    sql::ext::TypedTable<int64_t>                       fooTable(library->getDatabase().getTable(fooType.getName()));
    sql::ext::TypedTable<int64_t, int64_t, std::string> fooStringsTable(
      library->getDatabase().getTable(fooType.getName() + "_strings"));
    sql::ext::TypedTable<int64_t>                       barTable(library->getDatabase().getTable(barType.getName()));
    sql::ext::TypedTable<int64_t, int64_t, std::string> barStrings1Table(
      library->getDatabase().getTable(barType.getName() + "_strings1"));
    sql::ext::TypedTable<int64_t, int64_t, std::string> barStrings2Table(
      library->getDatabase().getTable(barType.getName() + "_strings2"));

    // Create object handlers.
    auto fooHandler = library->createObjectHandler<&Foo::id, &Foo::strings>(fooType.getName());
    auto barHandler = library->createObjectHandler<&Bar::id, &Bar::strings1, &Bar::strings2>(barType.getName());

    // Insert Foo.
    {
        // Create objects.
        Foo foo0;
        foo0.strings.get().push_back("abc");
        foo0.strings.get().push_back("def");
        Foo foo1;
        foo1.strings.get().push_back("10");
        foo1.strings.get().push_back("1111");
        foo1.strings.get().push_back("%^&*&(*U");

        // Try to insert.
        expectNoThrow([&] { fooHandler.insert(foo0); }).fatal("Failed to insert object");
        expectNoThrow([&] { fooHandler.insert(foo1); }).fatal("Failed to insert object");

        // Check assigned IDs.
        compareEQ(foo0.id, static_cast<int64_t>(1));
        compareEQ(foo1.id, static_cast<int64_t>(2));

        // Select inserted object using sql.
        auto foo0_get = fooTable.selectOne(fooTable.col<0>() == foo0.id, true)(false);
        auto foo1_get = fooTable.selectOne(fooTable.col<0>() == foo1.id, true)(false);

        // Compare objects.
        compareEQ(foo0.id, std::get<0>(foo0_get));
        compareEQ(foo1.id, std::get<0>(foo1_get));

        // Select floats in separate table.
        auto idparam        = foo0.id;
        auto strings_select = fooStringsTable.select<std::string, 2>(fooStringsTable.col<1>() == &idparam, true);
        std::vector<std::string> strings_get(strings_select.begin(), strings_select.end());
        compareEQ(foo0.strings.get(), strings_get);
        idparam = foo1.id;
        strings_get.assign(strings_select(true).begin(), strings_select.end());
        compareEQ(foo1.strings.get(), strings_get);
    }

    // Insert Bar.
    {
        // Create objects.
        Bar bar0;
        Bar bar1;
        bar1.strings1.get().push_back("");
        bar1.strings1.get().push_back("");
        bar1.strings1.get().push_back("hntfdrgtef");
        bar1.strings2.get().push_back("dbsfdcesw");
        bar1.strings2.get().push_back("utikrt");
        bar1.strings2.get().push_back("hntfdrgtef");

        // Try to insert.
        expectNoThrow([&] { barHandler.insert(bar0); }).fatal("Failed to insert object");
        expectNoThrow([&] { barHandler.insert(bar1); }).fatal("Failed to insert object");

        // Check assigned IDs.
        compareEQ(bar0.id, static_cast<int64_t>(1));
        compareEQ(bar1.id, static_cast<int64_t>(2));

        // Select inserted object using sql.
        auto bar0_get = barTable.selectOne(barTable.col<0>() == bar0.id, true)(false);
        auto bar1_get = barTable.selectOne(barTable.col<0>() == bar1.id, true)(false);

        // Compare objects.
        compareEQ(bar0.id, std::get<0>(bar0_get));
        compareEQ(bar1.id, std::get<0>(bar1_get));

        // Select ints in separate table.
        auto idparam         = bar0.id;
        auto strings1_select = barStrings1Table.select<std::string, 2>(barStrings1Table.col<1>() == &idparam, true);
        auto strings2_select = barStrings2Table.select<std::string, 2>(barStrings2Table.col<1>() == &idparam, true);
        std::vector<std::string> strings1_get(strings1_select.begin(), strings1_select.end());
        std::vector<std::string> strings2_get(strings2_select.begin(), strings2_select.end());
        compareEQ(bar0.strings1.get(), strings1_get);
        compareEQ(bar0.strings2.get(), strings2_get);
        idparam = bar1.id;
        strings1_get.assign(strings1_select(true).begin(), strings1_select.end());
        strings2_get.assign(strings2_select(true).begin(), strings2_select.end());
        compareEQ(bar1.strings1.get(), strings1_get);
        compareEQ(bar1.strings2.get(), strings2_get);
    }
}
