#include "alexandria-extended-query_test/table_sets/table_sets_nested.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria-basic-query/insert_query.h"
#include "alexandria-core/type_descriptor.h"
#include "alexandria-extended-query/table_sets.h"

namespace
{
    struct Foo
    {
        int32_t a = 0;
        int32_t b = 0;
    };

    struct Bar
    {
        alex::InstanceId id;
        Foo              foo;
    };

    using NestedMemberList = alex::MemberList<alex::Member<"a", &Foo::a>, alex::Member<"b", &Foo::b>>;
    using BarDescriptor    = alex::GenerateTypeDescriptor<alex::Member<"id", &Bar::id>,
                                                       alex::NestedMember<"foo", NestedMemberList, &Bar::foo>>;

}  // namespace

void TableSetsNested::operator()()
{
    expectNoThrow([&] {
        alex::TypeLayout fooLayout;
        fooLayout.createPrimitiveProperty("prop0", alex::DataType::Int32);
        fooLayout.createPrimitiveProperty("prop1", alex::DataType::Int32);
        fooLayout.commit(*nameSpace, "foo", alex::TypeLayout::Instantiable::False);

        alex::TypeLayout barLayout;
        barLayout.createNestedTypeProperty("prop0", nameSpace->getType("foo"));
        barLayout.commit(*nameSpace, "bar");
    }).fatal("Failed to commit types");

    auto& barType = nameSpace->getType("bar");

    {
        auto tableSets = alex::TableSets(BarDescriptor(barType));
        static_cast<void>(tableSets);

        compareEQ(0, decltype(tableSets)::primitive_array_table_set_t::size);
        compareEQ(0, decltype(tableSets)::blob_array_table_set_t::size);
        compareEQ(0, decltype(tableSets)::reference_array_table_set_t::size);

        compareTrue(std::is_same_v<decltype(tableSets.getInstanceTable().col<1>()),
                                   decltype(tableSets.getInstanceColumn<"id">())>);
        compareTrue(std::is_same_v<decltype(tableSets.getInstanceTable().col<2>()),
                                   decltype(tableSets.getInstanceColumn<"foo.a">())>);
        compareTrue(std::is_same_v<decltype(tableSets.getInstanceTable().col<3>()),
                                   decltype(tableSets.getInstanceColumn<"foo.b">())>);
    }
}
