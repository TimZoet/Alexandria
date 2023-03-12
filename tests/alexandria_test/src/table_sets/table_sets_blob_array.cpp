#include "alexandria_test/table_sets/table_sets_blob_array.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria/core/type_descriptor.h"
#include "alexandria/queries/table_sets.h"

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
    // Create type with 1 blob.
    auto& fooType = nameSpace->createType("foo");
    fooType.createBlobArrayProperty("blob1");

    // Create type with 2 blobs.
    auto& barType = nameSpace->createType("bar");
    barType.createBlobArrayProperty("blob1");
    barType.createBlobArrayProperty("blob2");

    // Commit types.
    expectNoThrow([&] {
        fooType.commit();
        barType.commit();
    }).fatal("Failed to commit types");

    // Check Foo.
    {
        auto  tableSets = alex::TableSets(FooDescriptor(fooType));
        auto& table0    = tableSets.getBlobArrayTable<0>();
        static_cast<void>(table0);

        compareEQ(0, decltype(tableSets)::primitive_array_table_set_t::size);
        compareEQ(1, decltype(tableSets)::blob_array_table_set_t::size);
        compareEQ(0, decltype(tableSets)::reference_array_table_set_t::size);

        compareTrue(std::is_same_v<decltype(tableSets.getInstanceTable().col<0>()),
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

        compareTrue(std::is_same_v<decltype(tableSets.getInstanceTable().col<0>()),
                                   decltype(tableSets.getInstanceColumn<"id">())>);
        compareTrue(
          std::is_same_v<decltype(tableSets.getBlobArrayTable<0>()), decltype(tableSets.getBlobArrayTable<"a">())>);
        compareTrue(
          std::is_same_v<decltype(tableSets.getBlobArrayTable<1>()), decltype(tableSets.getBlobArrayTable<"b">())>);
    }
}
