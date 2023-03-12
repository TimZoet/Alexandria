#include "alexandria_test/table_sets/table_sets_string_array.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria/core/type_descriptor.h"
#include "alexandria/queries/table_sets.h"

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

void TableSetsStringArray::operator()()
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

    // Check Foo.
    {
        auto  tableSets = alex::TableSets(FooDescriptor(fooType));
        auto& table0    = tableSets.getPrimitiveArrayTable<0>();
        static_cast<void>(table0);

        compareEQ(1, decltype(tableSets)::primitive_array_table_set_t::size);
        compareEQ(0, decltype(tableSets)::blob_array_table_set_t::size);
        compareEQ(0, decltype(tableSets)::reference_array_table_set_t::size);

        compareTrue(std::is_same_v<decltype(tableSets.getInstanceTable().col<0>()),
                                   decltype(tableSets.getInstanceColumn<"id">())>);
        compareTrue(std::is_same_v<decltype(tableSets.getPrimitiveArrayTable<0>()),
                                   decltype(tableSets.getPrimitiveArrayTable<"strings">())>);
    }

    // Check Bar.
    {
        auto  tableSets = alex::TableSets(BarDescriptor(barType));
        auto& table0    = tableSets.getPrimitiveArrayTable<0>();
        auto& table1    = tableSets.getPrimitiveArrayTable<1>();
        static_cast<void>(table0);
        static_cast<void>(table1);

        compareEQ(2, decltype(tableSets)::primitive_array_table_set_t::size);
        compareEQ(0, decltype(tableSets)::blob_array_table_set_t::size);
        compareEQ(0, decltype(tableSets)::reference_array_table_set_t::size);

        compareTrue(std::is_same_v<decltype(tableSets.getInstanceTable().col<0>()),
                                   decltype(tableSets.getInstanceColumn<"id">())>);
        compareTrue(std::is_same_v<decltype(tableSets.getPrimitiveArrayTable<0>()),
                                   decltype(tableSets.getPrimitiveArrayTable<"strings1">())>);
        compareTrue(std::is_same_v<decltype(tableSets.getPrimitiveArrayTable<1>()),
                                   decltype(tableSets.getPrimitiveArrayTable<"strings2">())>);
    }
}
