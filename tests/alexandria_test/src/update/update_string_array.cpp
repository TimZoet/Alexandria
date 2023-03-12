#include "alexandria_test/update/update_string_array.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria/core/type_descriptor.h"
#include "alexandria/queries/get_query.h"
#include "alexandria/queries/insert_query.h"
#include "alexandria/queries/update_query.h"

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

void UpdateStringArray::operator()()
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

    // Update Foo.
    {
        auto inserter = alex::InsertQuery(FooDescriptor(fooType));
        auto updater  = alex::UpdateQuery(FooDescriptor(fooType));
        auto getter   = alex::GetQuery(FooDescriptor(fooType));

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

        // Modify objects.
        foo0.strings.get().clear();
        foo1.strings.get().emplace_back("dgesrfd");

        // Try to update.
        expectNoThrow([&] { updater(foo0); }).fatal("Failed to update object");
        expectNoThrow([&] { updater(foo1); }).fatal("Failed to update object");

        // Try to retrieve.
        Foo foo0_get, foo1_get;
        foo0_get.id = foo0.id;
        foo1_get.id = foo1.id;
        expectNoThrow([&] { getter(foo0_get); }).fatal("Failed to retrieve object");
        expectNoThrow([&] { getter(foo1_get); }).fatal("Failed to retrieve object");

        // Compare objects.
        compareEQ(foo0.strings.get(), foo0_get.strings.get());
        compareEQ(foo1.strings.get(), foo1_get.strings.get());
    }

    // Update Bar.
    {
        auto inserter = alex::InsertQuery(BarDescriptor(barType));
        auto updater  = alex::UpdateQuery(BarDescriptor(barType));
        auto getter   = alex::GetQuery(BarDescriptor(barType));

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

        // Modify objects.
        bar0.strings2.get().emplace_back("oiuqdq");
        bar1.strings1.get().clear();
        bar1.strings1.get().emplace_back("erge");

        // Try to update.
        expectNoThrow([&] { updater(bar0); }).fatal("Failed to update object");
        expectNoThrow([&] { updater(bar1); }).fatal("Failed to update object");

        // Try to retrieve.
        Bar bar0_get, bar1_get;
        bar0_get.id = bar0.id;
        bar1_get.id = bar1.id;
        expectNoThrow([&] { getter(bar0_get); }).fatal("Failed to retrieve object");
        expectNoThrow([&] { getter(bar1_get); }).fatal("Failed to retrieve object");

        // Compare objects.
        compareEQ(bar0.strings1.get(), bar0_get.strings1.get());
        compareEQ(bar0.strings2.get(), bar0_get.strings2.get());
        compareEQ(bar1.strings1.get(), bar1_get.strings1.get());
        compareEQ(bar1.strings2.get(), bar1_get.strings2.get());
    }
}
