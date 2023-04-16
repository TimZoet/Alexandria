#include "alexandria-extended-query_test/table_sets/table_sets_primitive_blob.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria-core/type_descriptor.h"
#include "alexandria-extended-query/table_sets.h"

namespace
{
    struct Foo
    {
        alex::InstanceId           id;
        alex::PrimitiveBlob<float> floats;

        Foo() = default;

        Foo(const std::string& iid, std::vector<float> ffloats) : id(iid) { floats.get() = std::move(ffloats); }
    };

    struct Bar
    {
        alex::InstanceId             id;
        alex::PrimitiveBlob<int32_t> ints;

        Bar() = default;

        Bar(const std::string& iid, std::vector<int32_t> iints) : id(iid) { ints.get() = std::move(iints); }
    };

    struct Baz
    {
        alex::InstanceId              id;
        alex::PrimitiveBlob<uint64_t> uints;
        alex::PrimitiveBlob<double>   doubles;

        Baz() = default;

        Baz(const std::string& iid, std::vector<uint64_t> iints, std::vector<double> ffloats) : id(iid)
        {
            uints.get()   = std::move(iints);
            doubles.get() = std::move(ffloats);
        }
    };

    using FooDescriptor =
      alex::GenerateTypeDescriptor<alex::Member<"id", &Foo::id>, alex::Member<"floats", &Foo::floats>>;

    using BarDescriptor = alex::GenerateTypeDescriptor<alex::Member<"id", &Bar::id>, alex::Member<"ints", &Bar::ints>>;

    using BazDescriptor = alex::GenerateTypeDescriptor<alex::Member<"id", &Baz::id>,
                                                       alex::Member<"uints", &Baz::uints>,
                                                       alex::Member<"doubles", &Baz::doubles>>;
}  // namespace

void TableSetsPrimitiveBlob::operator()()
{
    expectNoThrow([&] {
        alex::TypeLayout fooLayout;
        fooLayout.createPrimitiveBlobProperty("prop0", alex::DataType::Float);
        fooLayout.commit(*nameSpace, "foo");

        alex::TypeLayout barLayout;
        barLayout.createPrimitiveBlobProperty("prop0", alex::DataType::Int32);
        barLayout.commit(*nameSpace, "bar");

        alex::TypeLayout bazLayout;
        bazLayout.createPrimitiveBlobProperty("prop0", alex::DataType::Uint64);
        bazLayout.createPrimitiveBlobProperty("prop1", alex::DataType::Double);
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

        compareTrue(std::is_same_v<decltype(tableSets.getInstanceTable().col<1>()),
                                   decltype(tableSets.getInstanceColumn<"id">())>);
        compareTrue(std::is_same_v<decltype(tableSets.getInstanceTable().col<2>()),
                                   decltype(tableSets.getInstanceColumn<"floats">())>);
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
                                   decltype(tableSets.getInstanceColumn<"ints">())>);
    }

    // Check Baz.
    {
        auto tableSets = alex::TableSets(BazDescriptor(bazType));
        static_cast<void>(tableSets);

        compareEQ(0, decltype(tableSets)::primitive_array_table_set_t::size);
        compareEQ(0, decltype(tableSets)::blob_array_table_set_t::size);
        compareEQ(0, decltype(tableSets)::reference_array_table_set_t::size);

        compareTrue(std::is_same_v<decltype(tableSets.getInstanceTable().col<1>()),
                                   decltype(tableSets.getInstanceColumn<"id">())>);
        compareTrue(std::is_same_v<decltype(tableSets.getInstanceTable().col<2>()),
                                   decltype(tableSets.getInstanceColumn<"uints">())>);
        compareTrue(std::is_same_v<decltype(tableSets.getInstanceTable().col<3>()),
                                   decltype(tableSets.getInstanceColumn<"doubles">())>);
    }
}
