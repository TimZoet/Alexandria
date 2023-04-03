#include "alexandria_test/table_sets/table_sets_nested.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria-core/type_descriptor.h"
#include "alexandria-query/insert_query.h"
#include "alexandria-query/table_sets.h"

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
    // Create types.
    auto& fooType = nameSpace->createType("foo", false);
    auto& barType = nameSpace->createType("bar");

    // Add properties to types.
    fooType.createPrimitiveProperty("a", alex::DataType::Int32);
    fooType.createPrimitiveProperty("b", alex::DataType::Int32);
    barType.createNestedTypeProperty("foo", fooType);

    // Commit types.
    expectNoThrow([&] {
        fooType.commit();
        barType.commit();
    }).fatal("Failed to commit types");

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
