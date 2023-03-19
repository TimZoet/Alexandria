#include "alexandria_test/delete/delete_string_array.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria-core/core/library.h"
#include "alexandria-core/core/type_descriptor.h"
#include "alexandria-query/queries/delete_query.h"
#include "alexandria-query/queries/insert_query.h"

namespace
{
    struct Foo
    {
        alex::InstanceId  id;
        alex::StringArray strings;

        Foo() = default;

        Foo(const alex::InstanceId iid, std::vector<std::string> sstrings) : id(iid)
        {
            strings.get() = std::move(sstrings);
        }
    };

    struct Bar
    {
        alex::InstanceId  id;
        alex::StringArray strings1;
        alex::StringArray strings2;

        Bar() = default;

        Bar(const alex::InstanceId iid, std::vector<std::string> sstrings1, std::vector<std::string> sstrings2) :
            id(iid)
        {
            strings1.get() = std::move(sstrings1);
            strings2.get() = std::move(sstrings2);
        }
    };

    using FooDescriptor =
      alex::GenerateTypeDescriptor<alex::Member<"id", &Foo::id>, alex::Member<"strings", &Foo::strings>>;

    using BarDescriptor = alex::GenerateTypeDescriptor<alex::Member<"id", &Bar::id>,
                                                       alex::Member<"strings1", &Bar::strings1>,
                                                       alex::Member<"strings2", &Bar::strings2>>;
}  // namespace

void DeleteStringArray::operator()()
{
    // Create type with 1 string.
    auto& fooType = nameSpace->createType("foo");
    fooType.createStringArrayProperty("strings");

    // Create type with 2 strings.
    auto& barType = nameSpace->createType("bar");
    barType.createStringArrayProperty("strings1");
    barType.createStringArrayProperty("strings2");

    // Commit types.
    expectNoThrow([&] {
        fooType.commit();
        barType.commit();
    }).fatal("Failed to commit types");

    // Delete Foo.
    {
        const sql::TypedTable<sql::row_id, std::string, std::string> arrayTable(
          library->getDatabase().getTable("main_foo_strings"));

        auto inserter = alex::InsertQuery(FooDescriptor(fooType));
        auto deleter  = alex::DeleteQuery(FooDescriptor(fooType));

        // Create objects.
        Foo foo0;
        foo0.strings.get().emplace_back("abc");
        foo0.strings.get().emplace_back("def");
        Foo foo1;
        foo1.strings.get().emplace_back("10");
        foo1.strings.get().emplace_back("1111");
        foo1.strings.get().emplace_back("%^&*&(*U");

        // Try to insert.
        expectNoThrow([&] { inserter(foo0); }).fatal("Failed to insert object");
        expectNoThrow([&] { inserter(foo1); }).fatal("Failed to insert object");

        // Verify existence of objects before and after delete.
        std::string id;
        auto        stmt = arrayTable.count().where(like(arrayTable.col<1>(), &id)).compile();
        id               = foo0.id.getAsString();
        compareEQ(2, stmt.bind(sql::BindParameters::All)());
        expectNoThrow([&] { deleter(foo0); });
        compareEQ(0, stmt.bind(sql::BindParameters::All)());
        id = foo1.id.getAsString();
        compareEQ(3, stmt.bind(sql::BindParameters::All)());
        expectNoThrow([&] { deleter(foo1); });
        compareEQ(0, stmt.bind(sql::BindParameters::All)());
    }

    // Insert Bar.
    {
        const sql::TypedTable<sql::row_id, std::string, std::string> array0Table(
          library->getDatabase().getTable("main_bar_strings1"));
        const sql::TypedTable<sql::row_id, std::string, std::string> array1Table(
          library->getDatabase().getTable("main_bar_strings2"));

        auto inserter = alex::InsertQuery(BarDescriptor(barType));
        auto deleter  = alex::DeleteQuery(BarDescriptor(barType));

        // Create objects.
        Bar bar0;
        Bar bar1;
        bar1.strings1.get().emplace_back("");
        bar1.strings1.get().emplace_back("");
        bar1.strings1.get().emplace_back("hntfdrgtef");
        bar1.strings2.get().emplace_back("dbsfdcesw");
        bar1.strings2.get().emplace_back("utikrt");
        bar1.strings2.get().emplace_back("hntfdrgtef");

        // Try to insert.
        expectNoThrow([&] { inserter(bar0); }).fatal("Failed to insert object");
        expectNoThrow([&] { inserter(bar1); }).fatal("Failed to insert object");

        // Verify existence of objects before and after delete.
        std::string id;
        auto        stmt0 = array0Table.count().where(like(array0Table.col<1>(), &id)).compile();
        auto        stmt1 = array1Table.count().where(like(array1Table.col<1>(), &id)).compile();
        id                = bar0.id.getAsString();
        compareEQ(0, stmt0.bind(sql::BindParameters::All)());
        compareEQ(0, stmt1.bind(sql::BindParameters::All)());
        expectNoThrow([&] { deleter(bar0); });
        compareEQ(0, stmt0.bind(sql::BindParameters::All)());
        compareEQ(0, stmt1.bind(sql::BindParameters::All)());
        id = bar1.id.getAsString();
        compareEQ(3, stmt0.bind(sql::BindParameters::All)());
        compareEQ(3, stmt1.bind(sql::BindParameters::All)());
        expectNoThrow([&] { deleter(bar1); });
        compareEQ(0, stmt0.bind(sql::BindParameters::All)());
        compareEQ(0, stmt1.bind(sql::BindParameters::All)());
    }
}
