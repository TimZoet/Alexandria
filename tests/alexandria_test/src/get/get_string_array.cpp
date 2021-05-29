#include "alexandria_test/get/get_string_array.h"

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

void GetStringArray::operator()()
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

    // Create object handlers.
    auto fooHandler =
      library->createObjectHandler<alex::Member<&Foo::id>, alex::Member<&Foo::strings>>(fooType.getName());
    auto barHandler =
      library->createObjectHandler<alex::Member<&Bar::id>, alex::Member<&Bar::strings1>, alex::Member<&Bar::strings2>>(
        barType.getName());

    // Retrieve Foo.
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

        // Try to retrieve objects.
        Foo foo0_get, foo1_get;
        expectNoThrow([&] { fooHandler->get(foo0.id, foo0_get); }).fatal("Failed to get object");
        expectNoThrow([&] { fooHandler->get(foo1.id, foo1_get); }).fatal("Failed to get object");

        // Compare objects.
        compareEQ(foo0.id, foo0_get.id);
        compareEQ(foo0.strings.get(), foo0_get.strings.get());
        compareEQ(foo1.id, foo1_get.id);
        compareEQ(foo1.strings.get(), foo1_get.strings.get());
    }

    // Retrieve Bar.
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

        // Try to retrieve objects.
        Bar bar0_get, bar1_get;
        expectNoThrow([&] { barHandler->get(bar0.id, bar0_get); }).fatal("Failed to get object");
        expectNoThrow([&] { barHandler->get(bar1.id, bar1_get); }).fatal("Failed to get object");

        // Compare objects.
        compareEQ(bar0.id, bar0_get.id);
        compareEQ(bar0.strings1.get(), bar0_get.strings1.get());
        compareEQ(bar0.strings2.get(), bar0_get.strings2.get());
        compareEQ(bar1.id, bar1_get.id);
        compareEQ(bar1.strings1.get(), bar1_get.strings1.get());
        compareEQ(bar1.strings2.get(), bar1_get.strings2.get());
    }
}
