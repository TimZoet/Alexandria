#include "alexandria_test/update/update_blob_array.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria/core/type_descriptor.h"
#include "alexandria/queries/get_query.h"
#include "alexandria/queries/insert_query.h"
#include "alexandria/queries/update_query.h"

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

    using FooDescriptor = alex::GenerateTypeDescriptor<alex::Member<&Foo::id>, alex::Member<&Foo::a>>;

    using BarDescriptor =
      alex::GenerateTypeDescriptor<alex::Member<&Bar::id>, alex::Member<&Bar::a>, alex::Member<&Bar::b>>;
}  // namespace

void UpdateBlobArray::operator()()
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

    // Update Foo.
    {
        auto inserter = alex::InsertQuery(FooDescriptor(fooType));
        auto updater  = alex::UpdateQuery(FooDescriptor(fooType));
        auto getter   = alex::GetQuery(FooDescriptor(fooType));

        // Create objects.
        Foo foo0;
        foo0.a.get().emplace_back(Baz{.x = 0.5f, .y = 10});
        foo0.a.get().emplace_back(Baz{.x = -2.0f, .y = -32});
        Foo foo1;
        foo1.a.get().emplace_back(Baz{.x = 4.5f, .y = 10000});

        // Try to insert.
        expectNoThrow([&] { inserter(foo0); }).fatal("Failed to insert object");
        expectNoThrow([&] { inserter(foo1); }).fatal("Failed to insert object");

        // Modify objects.
        foo0.a.get().clear();
        foo0.a.get().emplace_back(Baz{.x = 3.5f, .y = -22});
        foo1.a.get().clear();

        // Try to update.
        expectNoThrow([&] { updater(foo0); }).fatal("Failed to update object");
        expectNoThrow([&] { updater(foo1); }).fatal("Failed to update object");

        // Try to retrieve.
        Foo foo0_get, foo1_get;
        foo0_get.id = foo0.id;
        foo1_get.id = foo1.id;
        expectNoThrow([&] { getter(foo0_get); }).fatal("Failed to retrieve object");
        expectNoThrow([&] { getter(foo1_get); }).fatal("Failed to retrieve object");

        // Compare objects.
        compareEQ(foo0.a.get(), foo0_get.a.get());
        compareEQ(foo1.a.get(), foo1_get.a.get());
    }

    // Update Bar.
    {
        auto inserter = alex::InsertQuery(BarDescriptor(barType));
        auto updater  = alex::UpdateQuery(BarDescriptor(barType));
        auto getter   = alex::GetQuery(BarDescriptor(barType));

        // Create objects.
        Bar bar0;
        bar0.a.get().resize(2);
        bar0.a.get()[0].emplace_back(Baz{.x = 0.5f, .y = 10});
        bar0.a.get()[0].emplace_back(Baz{.x = -2.0f, .y = -32});
        bar0.a.get()[1].emplace_back(Baz{.x = 33.0f, .y = 34});
        bar0.b.get().resize(3);
        bar0.b.get()[0].push_back(1.0f);
        bar0.b.get()[0].push_back(2.0f);
        bar0.b.get()[0].push_back(3.0f);
        bar0.b.get()[1].push_back(4.0f);
        bar0.b.get()[1].push_back(5.0f);
        bar0.b.get()[2].push_back(6.0f);
        Bar bar1;
        bar1.a.get().resize(1);
        bar1.a.get()[0].emplace_back(Baz{.x = 8.5f, .y = -50});
        bar1.b.get().resize(2);
        bar1.b.get()[0].push_back(11.0f);
        bar1.b.get()[0].push_back(12.0f);
        bar1.b.get()[0].push_back(13.0f);

        // Try to insert.
        expectNoThrow([&] { inserter(bar0); }).fatal("Failed to insert object");
        expectNoThrow([&] { inserter(bar1); }).fatal("Failed to insert object");

        // Modify objects.
        bar0.a.get()[0].clear();
        bar0.a.get()[1].emplace_back(Baz{.x = 0.5f, .y = 10});
        bar0.b.get()[0].clear();
        bar0.b.get()[1].push_back(2.0f);
        bar0.b.get()[1].push_back(2.0f);
        bar0.b.get()[1].push_back(2.0f);
        bar0.b.get()[1].push_back(4.0f);
        bar0.a.get().clear();
        bar0.b.get().resize(4);
        bar0.b.get()[2].push_back(3.0f);

        // Try to update.
        expectNoThrow([&] { updater(bar0); }).fatal("Failed to update object");
        expectNoThrow([&] { updater(bar1); }).fatal("Failed to update object");

        // Try to retrieve.
        Bar bar0_get, bar1_get;
        bar0_get.id = bar0.id;
        bar1_get.id = bar1.id;
        expectNoThrow([&] { getter(bar0_get); }).fatal("Failed to retrieve object");
        expectNoThrow([&] { getter(bar1_get); }).fatal("Failed to retrieve object");

        // Compare objects.
        compareEQ(bar0.a.get(), bar0_get.a.get());
        compareEQ(bar0.b.get(), bar0_get.b.get());
        compareEQ(bar1.a.get(), bar1_get.a.get());
        compareEQ(bar1.b.get(), bar1_get.b.get());
    }
}
