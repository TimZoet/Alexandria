#include "alexandria-basic-query_test/update/update_primitive_blob.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria-core/type_descriptor.h"
#include "alexandria-basic-query/get_query.h"
#include "alexandria-basic-query/insert_query.h"
#include "alexandria-basic-query/update_query.h"

namespace
{
    struct Foo
    {
        alex::InstanceId           id;
        alex::PrimitiveBlob<float> floats;

        Foo() = default;

        Foo(const alex::InstanceId iid, std::vector<float> ffloats) : id(iid) { floats.get() = std::move(ffloats); }
    };

    struct Bar
    {
        alex::InstanceId             id;
        alex::PrimitiveBlob<int32_t> ints;

        Bar() = default;

        Bar(const alex::InstanceId iid, std::vector<int32_t> iints) : id(iid) { ints.get() = std::move(iints); }
    };

    struct Baz
    {
        alex::InstanceId              id;
        alex::PrimitiveBlob<uint64_t> uints;
        alex::PrimitiveBlob<double>   doubles;

        Baz() = default;

        Baz(const alex::InstanceId iid, std::vector<uint64_t> iints, std::vector<double> ffloats) : id(iid)
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

void UpdatePrimitiveBlob::operator()()
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

    // Update Foo.
    {
        auto inserter = alex::InsertQuery(FooDescriptor(fooType));
        auto updater  = alex::UpdateQuery(FooDescriptor(fooType));
        auto getter   = alex::GetQuery(FooDescriptor(fooType));

        // Create objects.
        Foo foo0;
        foo0.floats.get().push_back(0.5f);
        foo0.floats.get().push_back(1.5f);
        Foo foo1;
        foo1.floats.get().push_back(-2.5f);
        foo1.floats.get().push_back(-3.5f);
        foo1.floats.get().push_back(-4.5f);

        // Try to insert.
        expectNoThrow([&] { inserter(foo0); }).fatal("Failed to insert object");
        expectNoThrow([&] { inserter(foo1); }).fatal("Failed to insert object");

        // Try to retrieve.
        Foo foo0_get(foo0.id, {});
        Foo foo1_get(foo1.id, {});
        expectNoThrow([&] { getter(foo0_get); }).fatal("Failed to retrieve object");
        expectNoThrow([&] { getter(foo1_get); }).fatal("Failed to retrieve object");

        // Compare objects.
        compareEQ(foo0.id, foo0_get.id);
        compareEQ(foo0.floats.get(), foo0_get.floats.get());
        compareEQ(foo1.id, foo1_get.id);
        compareEQ(foo1.floats.get(), foo1_get.floats.get());
    }

    // Update Bar.
    {
        auto inserter = alex::InsertQuery(BarDescriptor(barType));
        auto updater  = alex::UpdateQuery(BarDescriptor(barType));
        auto getter   = alex::GetQuery(BarDescriptor(barType));

        // Create objects.
        Bar bar0;
        bar0.ints.get().push_back(10);
        bar0.ints.get().push_back(100);
        Bar bar1;
        bar1.ints.get().push_back(-111);
        bar1.ints.get().push_back(-2222);
        bar1.ints.get().push_back(-33333);

        // Try to insert.
        expectNoThrow([&] { inserter(bar0); }).fatal("Failed to insert object");
        expectNoThrow([&] { inserter(bar1); }).fatal("Failed to insert object");

        // Try to retrieve.
        Bar bar0_get(bar0.id, {});
        Bar bar1_get(bar1.id, {});
        expectNoThrow([&] { getter(bar0_get); }).fatal("Failed to retrieve object");
        expectNoThrow([&] { getter(bar1_get); }).fatal("Failed to retrieve object");

        // Compare objects.
        compareEQ(bar0.id, bar0_get.id);
        compareEQ(bar0.ints.get(), bar0_get.ints.get());
        compareEQ(bar1.id, bar1_get.id);
        compareEQ(bar1.ints.get(), bar1_get.ints.get());
    }

    // Update Baz.
    {
        auto inserter = alex::InsertQuery(BazDescriptor(bazType));
        auto updater  = alex::UpdateQuery(BazDescriptor(bazType));
        auto getter   = alex::GetQuery(BazDescriptor(bazType));

        // Create objects.
        Baz baz0;
        baz0.uints.get().push_back(10);
        baz0.uints.get().push_back(100);
        baz0.doubles.get().push_back(0.5);
        baz0.doubles.get().push_back(1.5);
        Baz baz1;
        baz1.uints.get().push_back(111);
        baz1.uints.get().push_back(2222);
        baz1.uints.get().push_back(33333);
        baz1.doubles.get().push_back(-2.5);
        baz1.doubles.get().push_back(-3.5);
        baz1.doubles.get().push_back(-4.5);

        // Try to insert.
        expectNoThrow([&] { inserter(baz0); }).fatal("Failed to insert object");
        expectNoThrow([&] { inserter(baz1); }).fatal("Failed to insert object");

        // Modify objects.
        baz0.uints.get().push_back(25);
        baz0.doubles.get().push_back(5.0);
        baz1.uints.get().push_back(100);
        baz1.uints.get().push_back(200);
        baz1.uints.get().push_back(300);

        // Try to update.
        expectNoThrow([&] { updater(baz0); }).fatal("Failed to update object");
        expectNoThrow([&] { updater(baz1); }).fatal("Failed to update object");

        // Try to retrieve.
        Baz baz0_get, baz1_get;
        baz0_get.id = baz0.id;
        baz1_get.id = baz1.id;
        expectNoThrow([&] { getter(baz0_get); }).fatal("Failed to retrieve object");
        expectNoThrow([&] { getter(baz1_get); }).fatal("Failed to retrieve object");

        // Compare objects.
        compareEQ(baz0.uints.get(), baz0_get.uints.get());
        compareEQ(baz0.doubles.get(), baz0_get.doubles.get());
        compareEQ(baz1.uints.get(), baz1_get.uints.get());
        compareEQ(baz1.doubles.get(), baz1_get.doubles.get());
    }
}
