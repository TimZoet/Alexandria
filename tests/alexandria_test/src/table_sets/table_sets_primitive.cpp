#include "alexandria_test/table_sets/table_sets_primitive.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria/core/type_descriptor.h"
#include "alexandria/queries/table_sets.h"

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
    // Create type with floats.
    auto& fooType = nameSpace->createType("foo");
    fooType.createPrimitiveProperty("floatprop", alex::DataType::Float);
    fooType.createPrimitiveProperty("doubleprop", alex::DataType::Double);

    // Create type with integers.
    auto& barType = nameSpace->createType("bar");
    barType.createPrimitiveProperty("int32prop", alex::DataType::Int32);
    barType.createPrimitiveProperty("int64prop", alex::DataType::Int64);
    barType.createPrimitiveProperty("uint32prop", alex::DataType::Uint32);
    barType.createPrimitiveProperty("uint64prop", alex::DataType::Uint64);

    // Commit types.
    expectNoThrow([&] {
        fooType.commit();
        barType.commit();
    }).fatal("Failed to commit types");

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
