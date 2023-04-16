#include "alexandria-basic-query_test/delete/delete_string_array.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria-core/library.h"
#include "alexandria-core/type_descriptor.h"
#include "alexandria-basic-query/delete_query.h"
#include "alexandria-basic-query/insert_query.h"

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
    expectNoThrow([&] {
        alex::TypeLayout fooLayout;
        fooLayout.createStringArrayProperty("prop0");
        fooLayout.commit(*nameSpace, "foo");

        alex::TypeLayout barLayout;
        barLayout.createStringArrayProperty("prop0");
        barLayout.createStringArrayProperty("prop1");
        barLayout.commit(*nameSpace, "bar");
    }).fatal("Failed to commit types");

    auto& fooType = nameSpace->getType("foo");
    auto& barType = nameSpace->getType("bar");

    // Delete Foo.
    {
        const sql::TypedTable<sql::row_id, std::string, std::string> arrayTable(
          library->getDatabase().getTable("main_foo_prop0"));

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
          library->getDatabase().getTable("main_bar_prop0"));
        const sql::TypedTable<sql::row_id, std::string, std::string> array1Table(
          library->getDatabase().getTable("main_bar_prop1"));

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
