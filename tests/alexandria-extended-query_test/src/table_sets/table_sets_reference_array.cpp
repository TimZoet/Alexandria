#include "alexandria-extended-query_test/table_sets/table_sets_reference_array.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria-core/type_descriptor.h"
#include "alexandria-extended-query/table_sets.h"

namespace
{
    struct Foo
    {
        alex::InstanceId id;
        float            a;
        int32_t          b;
    };

    struct Bar
    {
        alex::InstanceId          id;
        alex::ReferenceArray<Foo> foo;
    };

    struct Baz
    {
        alex::InstanceId          id;
        alex::ReferenceArray<Foo> foo;
        alex::ReferenceArray<Bar> bar;
    };

    using FooDescriptor = alex::
      GenerateTypeDescriptor<alex::Member<"id", &Foo::id>, alex::Member<"a", &Foo::a>, alex::Member<"b", &Foo::b>>;

    using BarDescriptor = alex::GenerateTypeDescriptor<alex::Member<"id", &Bar::id>, alex::Member<"foo", &Bar::foo>>;

    using BazDescriptor = alex::GenerateTypeDescriptor<alex::Member<"id", &Baz::id>,
                                                       alex::Member<"foo", &Baz::foo>,
                                                       alex::Member<"bar", &Baz::bar>>;
}  // namespace

void TableSetsReferenceArray::operator()()
{
    expectNoThrow([&] {
        alex::TypeLayout fooLayout;
        fooLayout.createPrimitiveProperty("prop0", alex::DataType::Float);
        fooLayout.createPrimitiveProperty("prop1", alex::DataType::Int32);
        fooLayout.commit(*nameSpace, "foo");

        alex::TypeLayout barLayout;
        barLayout.createReferenceArrayProperty("prop0", nameSpace->getType("foo"));
        barLayout.commit(*nameSpace, "bar");

        alex::TypeLayout bazLayout;
        bazLayout.createReferenceArrayProperty("prop0", nameSpace->getType("foo"));
        bazLayout.createReferenceArrayProperty("prop1", nameSpace->getType("bar"));
        bazLayout.commit(*nameSpace, "baz");
    }).fatal("Failed to commit types");

    auto& fooType = nameSpace->getType("foo");
    auto& barType = nameSpace->getType("bar");
    auto& bazType = nameSpace->getType("baz");

    // Check Foo.
    {
        auto tableSets = alex::TableSets(FooDescriptor(fooType));
        static_cast<void>(tableSets);

        compareEQ(0, decltype(tableSets)::primitive_array_table_set_t::size);
        compareEQ(0, decltype(tableSets)::blob_array_table_set_t::size);
        compareEQ(0, decltype(tableSets)::reference_array_table_set_t::size);
    }

    // Check Bar.
    {
        auto  tableSets = alex::TableSets(BarDescriptor(barType));
        auto& table0    = tableSets.getReferenceArrayTable<0>();
        static_cast<void>(table0);

        compareEQ(0, decltype(tableSets)::primitive_array_table_set_t::size);
        compareEQ(0, decltype(tableSets)::blob_array_table_set_t::size);
        compareEQ(1, decltype(tableSets)::reference_array_table_set_t::size);

        compareTrue(std::is_same_v<decltype(tableSets.getInstanceTable().col<1>()),
                                   decltype(tableSets.getInstanceColumn<"id">())>);
        compareTrue(std::is_same_v<decltype(tableSets.getReferenceArrayTable<0>()),
                                   decltype(tableSets.getReferenceArrayTable<"foo">())>);
    }

    // Check Baz.
    {
        auto  tableSets = alex::TableSets(BazDescriptor(bazType));
        auto& table0    = tableSets.getReferenceArrayTable<0>();
        auto& table1    = tableSets.getReferenceArrayTable<1>();
        static_cast<void>(table0);
        static_cast<void>(table1);

        compareEQ(0, decltype(tableSets)::primitive_array_table_set_t::size);
        compareEQ(0, decltype(tableSets)::blob_array_table_set_t::size);
        compareEQ(2, decltype(tableSets)::reference_array_table_set_t::size);

        compareTrue(std::is_same_v<decltype(tableSets.getInstanceTable().col<1>()),
                                   decltype(tableSets.getInstanceColumn<"id">())>);
        compareTrue(std::is_same_v<decltype(tableSets.getReferenceArrayTable<0>()),
                                   decltype(tableSets.getReferenceArrayTable<"foo">())>);
        compareTrue(std::is_same_v<decltype(tableSets.getReferenceArrayTable<1>()),
                                   decltype(tableSets.getReferenceArrayTable<"bar">())>);
    }
}
