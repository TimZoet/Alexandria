#include "alexandria-basic-query_test/insert/insert_string_array.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria-core/library.h"
#include "alexandria-core/type_descriptor.h"
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

void InsertStringArray::operator()()
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

    // Insert Foo.
    {
        const sql::TypedTable<sql::row_id, std::string, std::string> arrayTable(
          library->getDatabase().getTable("main_foo_prop0"));

        auto inserter = alex::InsertQuery(FooDescriptor(fooType));

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

        // Check assigned IDs.
        compareTrue(foo0.id.valid());
        compareTrue(foo1.id.valid());

        // Select strings in array table.
        std::string id;
        auto        stmt = arrayTable.selectAs<std::string, 2>()
                      .where(like(arrayTable.col<1>(), &id))
                      .orderBy(ascending(arrayTable.col<0>()))
                      .compile();
        id = foo0.id.getAsString();
        stmt.bind(sql::BindParameters::All);
        std::vector<std::string> strings(stmt.begin(), stmt.end());
        compareEQ(foo0.strings.get(), strings);
        id = foo1.id.getAsString();
        stmt.bind(sql::BindParameters::All);
        strings.assign(stmt.begin(), stmt.end());
        compareEQ(foo1.strings.get(), strings);
    }

    // Insert Bar.
    {
        const sql::TypedTable<sql::row_id, std::string, std::string> array0Table(
          library->getDatabase().getTable("main_bar_prop0"));
        const sql::TypedTable<sql::row_id, std::string, std::string> array1Table(
          library->getDatabase().getTable("main_bar_prop1"));

        auto inserter = alex::InsertQuery(BarDescriptor(barType));

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

        // Check assigned IDs.
        compareTrue(bar0.id.valid());
        compareTrue(bar1.id.valid());

        // Select strings in array table.
        std::string id;
        auto        stmt0 = array0Table.selectAs<std::string, 2>()
                       .where(like(array0Table.col<1>(), &id))
                       .orderBy(ascending(array0Table.col<0>()))
                       .compile();
        auto stmt1 = array1Table.selectAs<std::string, 2>()
                       .where(like(array1Table.col<1>(), &id))
                       .orderBy(ascending(array1Table.col<0>()))
                       .compile();
        id = bar0.id.getAsString();
        stmt0.bind(sql::BindParameters::All);
        stmt1.bind(sql::BindParameters::All);
        std::vector<std::string> strings1(stmt0.begin(), stmt0.end());
        std::vector<std::string> strings2(stmt1.begin(), stmt1.end());
        compareEQ(bar0.strings1.get(), strings1);
        compareEQ(bar0.strings2.get(), strings2);
        id = bar1.id.getAsString();
        stmt0.bind(sql::BindParameters::All);
        stmt1.bind(sql::BindParameters::All);
        strings1.assign(stmt0.begin(), stmt0.end());
        strings2.assign(stmt1.begin(), stmt1.end());
        compareEQ(bar1.strings1.get(), strings1);
        compareEQ(bar1.strings2.get(), strings2);
    }
}
