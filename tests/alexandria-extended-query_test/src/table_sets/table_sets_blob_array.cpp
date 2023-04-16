#include "alexandria-extended-query_test/table_sets/table_sets_blob_array.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria-core/type_descriptor.h"
#include "alexandria-extended-query/table_sets.h"

namespace
{
    struct Baz
    {
        float   x;
        int32_t y;

        bool operator==(const Baz& rhs) const noexcept { return x == rhs.x && y == rhs.y; }

        friend std::ostream& operator<<(std::ostream& out, const Baz& baz)
        {
            return out << "(" << baz.x << ", " << baz.y << ")";
        }
    };

    struct Foo
    {
        alex::InstanceId     id;
        alex::BlobArray<Baz> a;
    };

    struct Bar
    {
        alex::InstanceId                    id;
        alex::BlobArray<std::vector<Baz>>   a;
        alex::BlobArray<std::vector<float>> b;
    };

    using FooDescriptor = alex::GenerateTypeDescriptor<alex::Member<"id", &Foo::id>, alex::Member<"a", &Foo::a>>;

    using BarDescriptor = alex::
      GenerateTypeDescriptor<alex::Member<"id", &Bar::id>, alex::Member<"a", &Bar::a>, alex::Member<"b", &Bar::b>>;
}  // namespace

void TableSetsBlobArray::operator()()
{
    expectNoThrow([&] {
        alex::TypeLayout fooLayout;
        fooLayout.createBlobArrayProperty("prop0");
        fooLayout.commit(*nameSpace, "foo");

        alex::TypeLayout barLayout;
        barLayout.createBlobArrayProperty("prop0");
        barLayout.createBlobArrayProperty("prop1");
        barLayout.commit(*nameSpace, "bar");
    }).fatal("Failed to commit types");

    auto& fooType = nameSpace->getType("foo");
    auto& barType = nameSpace->getType("bar");

    // Check Foo.
    {
        auto  tableSets = alex::TableSets(FooDescriptor(fooType));
        auto& table0    = tableSets.getBlobArrayTable<0>();
        static_cast<void>(table0);

        compareEQ(0, decltype(tableSets)::primitive_array_table_set_t::size);
        compareEQ(1, decltype(tableSets)::blob_array_table_set_t::size);
        compareEQ(0, decltype(tableSets)::reference_array_table_set_t::size);

        compareTrue(std::is_same_v<decltype(tableSets.getInstanceTable().col<1>()),
                                   decltype(tableSets.getInstanceColumn<"id">())>);
        compareTrue(
          std::is_same_v<decltype(tableSets.getBlobArrayTable<0>()), decltype(tableSets.getBlobArrayTable<"a">())>);
    }

    // Check Bar.
    {
        auto  tableSets = alex::TableSets(BarDescriptor(barType));
        auto& table0    = tableSets.getBlobArrayTable<0>();
        auto& table1    = tableSets.getBlobArrayTable<1>();
        static_cast<void>(table0);
        static_cast<void>(table1);

        compareEQ(0, decltype(tableSets)::primitive_array_table_set_t::size);
        compareEQ(2, decltype(tableSets)::blob_array_table_set_t::size);
        compareEQ(0, decltype(tableSets)::reference_array_table_set_t::size);

        compareTrue(std::is_same_v<decltype(tableSets.getInstanceTable().col<1>()),
                                   decltype(tableSets.getInstanceColumn<"id">())>);
        compareTrue(
          std::is_same_v<decltype(tableSets.getBlobArrayTable<0>()), decltype(tableSets.getBlobArrayTable<"a">())>);
        compareTrue(
          std::is_same_v<decltype(tableSets.getBlobArrayTable<1>()), decltype(tableSets.getBlobArrayTable<"b">())>);
    }
}
