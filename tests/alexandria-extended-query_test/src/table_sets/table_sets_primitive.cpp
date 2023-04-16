#include "alexandria-extended-query_test/table_sets/table_sets_primitive.h"

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
        float            a = 0;
        double           b = 0;

        Foo(const float aa, const double bb) : a(aa), b(bb) {}

        Foo(const std::string& iid, const float aa, const double bb) : id(iid), a(aa), b(bb) {}
    };

    struct Bar
    {
        alex::InstanceId id;
        int32_t          a = 0;
        int64_t          b = 0;
        uint32_t         c = 0;
        uint64_t         d = 0;

        Bar(const int32_t aa, const int64_t bb, const uint32_t cc, const uint64_t dd) : a(aa), b(bb), c(cc), d(dd) {}

        Bar(const std::string& iid, const int32_t aa, const int64_t bb, const uint32_t cc, const uint64_t dd) :
            id(iid), a(aa), b(bb), c(cc), d(dd)
        {
        }
    };

    using FooDescriptor = alex::
      GenerateTypeDescriptor<alex::Member<"id", &Foo::id>, alex::Member<"a", &Foo::a>, alex::Member<"b", &Foo::b>>;

    using BarDescriptor = alex::GenerateTypeDescriptor<alex::Member<"id", &Bar::id>,
                                                       alex::Member<"a", &Bar::a>,
                                                       alex::Member<"b", &Bar::b>,
                                                       alex::Member<"c", &Bar::c>,
                                                       alex::Member<"d", &Bar::d>>;
}  // namespace

void TableSetsPrimitive::operator()()
{
    expectNoThrow([&] {
        alex::TypeLayout fooLayout;
        fooLayout.createPrimitiveProperty("prop0", alex::DataType::Float);
        fooLayout.createPrimitiveProperty("prop1", alex::DataType::Double);
        fooLayout.commit(*nameSpace, "foo");

        alex::TypeLayout barLayout;
        barLayout.createPrimitiveProperty("prop0", alex::DataType::Int32);
        barLayout.createPrimitiveProperty("prop1", alex::DataType::Int64);
        barLayout.createPrimitiveProperty("prop2", alex::DataType::Uint32);
        barLayout.createPrimitiveProperty("prop3", alex::DataType::Uint64);
        barLayout.commit(*nameSpace, "bar");
    }).fatal("Failed to commit types");

    auto& fooType = nameSpace->getType("foo");
    auto& barType = nameSpace->getType("bar");

    // Check Foo.
    {
        auto tableSets = alex::TableSets(FooDescriptor(fooType));
        static_cast<void>(tableSets);

        compareEQ(0, decltype(tableSets)::primitive_array_table_set_t::size);
        compareEQ(0, decltype(tableSets)::blob_array_table_set_t::size);
        compareEQ(0, decltype(tableSets)::reference_array_table_set_t::size);

        compareTrue(std::is_same_v<decltype(tableSets.getInstanceTable().col<1>()),
                                   decltype(tableSets.getInstanceColumn<"id">())>);
        compareTrue(std::is_same_v<decltype(tableSets.getInstanceTable().col<2>()),
                                   decltype(tableSets.getInstanceColumn<"a">())>);
        compareTrue(std::is_same_v<decltype(tableSets.getInstanceTable().col<3>()),
                                   decltype(tableSets.getInstanceColumn<"b">())>);
    }

    // Check Bar.
    {
        auto tableSets = alex::TableSets(BarDescriptor(barType));
        static_cast<void>(tableSets);

        compareEQ(0, decltype(tableSets)::primitive_array_table_set_t::size);
        compareEQ(0, decltype(tableSets)::blob_array_table_set_t::size);
        compareEQ(0, decltype(tableSets)::reference_array_table_set_t::size);

        compareTrue(std::is_same_v<decltype(tableSets.getInstanceTable().col<1>()),
                                   decltype(tableSets.getInstanceColumn<"id">())>);
        compareTrue(std::is_same_v<decltype(tableSets.getInstanceTable().col<2>()),
                                   decltype(tableSets.getInstanceColumn<"a">())>);
        compareTrue(std::is_same_v<decltype(tableSets.getInstanceTable().col<3>()),
                                   decltype(tableSets.getInstanceColumn<"b">())>);
        compareTrue(std::is_same_v<decltype(tableSets.getInstanceTable().col<4>()),
                                   decltype(tableSets.getInstanceColumn<"c">())>);
        compareTrue(std::is_same_v<decltype(tableSets.getInstanceTable().col<5>()),
                                   decltype(tableSets.getInstanceColumn<"d">())>);
    }
}
