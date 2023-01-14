#include "alexandria_test/update/update_blob.h"

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
        alex::InstanceId id;
        alex::Blob<Baz>  a{};
    };

    struct Bar
    {
        alex::InstanceId               id;
        alex::Blob<std::vector<Baz>>   a;
        alex::Blob<std::vector<float>> b;
    };

    using FooDescriptor = alex::GenerateTypeDescriptor<alex::Member<&Foo::id>, alex::Member<&Foo::a>>;

    using BarDescriptor =
      alex::GenerateTypeDescriptor<alex::Member<&Bar::id>, alex::Member<&Bar::a>, alex::Member<&Bar::b>>;
}  // namespace

void UpdateBlob::operator()()
{
    // Create type with 1 blob.
    auto& fooType = nameSpace->createType("Foo");
    fooType.createBlobProperty("blobProp1");

    // Create type with 2 blobs.
    auto& barType = nameSpace->createType("Bar");
    barType.createBlobProperty("blobProp1");
    barType.createBlobProperty("blobProp2");

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
        foo0.a.get().x = 3.5f;
        foo0.a.get().y = 12;
        Foo foo1;
        foo1.a.get().x = -0.5f;
        foo1.a.get().y = -10;

        // Try to insert.
        expectNoThrow([&] { inserter(foo0); }).fatal("Failed to insert object");
        expectNoThrow([&] { inserter(foo1); }).fatal("Failed to insert object");

        // Modify objects.
        foo0.a.get().x = 33.0f;
        foo0.a.get().y = 44;
        foo1.a.get().x = 1.55f;
        foo1.a.get().y = 4;

        // Try to update.
        expectNoThrow([&] { updater(foo0); }).fatal("Failed to update object");
        expectNoThrow([&] { updater(foo1); }).fatal("Failed to update object");

        // Try to retrieve.
        Foo foo0_get{.id = foo0.id};
        Foo foo1_get{.id = foo1.id};
        expectNoThrow([&] { getter(foo0_get); }).fatal("Failed to retrieve object");
        expectNoThrow([&] { getter(foo1_get); }).fatal("Failed to retrieve object");

        // Compare objects.
        compareEQ(foo0.id, foo0_get.id);
        compareEQ(foo0.a.get(), foo0_get.a.get());
        compareEQ(foo1.id, foo1_get.id);
        compareEQ(foo1.a.get(), foo1_get.a.get());
    }

    // Update Bar.
    {
        auto inserter = alex::InsertQuery(BarDescriptor(barType));
        auto updater  = alex::UpdateQuery(BarDescriptor(barType));
        auto getter   = alex::GetQuery(BarDescriptor(barType));

        // Create objects.
        Bar bar0;
        bar0.a.get().push_back(Baz{.x = 1.0f, .y = 2});
        bar0.a.get().push_back(Baz{.x = -1.0f, .y = -13});
        bar0.b.get().push_back(33.0f);
        bar0.b.get().push_back(42.0f);
        Bar bar1;
        bar1.a.get().push_back(Baz{.x = 44.0f, .y = -11111});
        bar1.b.get().push_back(-10.0f);
        bar1.b.get().push_back(-20.0f);
        bar1.b.get().push_back(-30.0f);

        // Try to insert.
        expectNoThrow([&] { inserter(bar0); }).fatal("Failed to insert object");
        expectNoThrow([&] { inserter(bar1); }).fatal("Failed to insert object");

        // Modify objects.
        bar0.a.get().clear();
        bar0.b.get().push_back(-14.0f);
        bar1.a.get().push_back(Baz{.x = -10.0f, .y = 333});
        bar1.b.get().erase(bar1.b.get().begin());
        bar1.b.get().push_back(40.0f);

        // Try to update.
        expectNoThrow([&] { updater(bar0); }).fatal("Failed to update object");
        expectNoThrow([&] { updater(bar1); }).fatal("Failed to update object");

        // Try to retrieve.
        Bar bar0_get{.id = bar0.id};
        Bar bar1_get{.id = bar1.id};
        expectNoThrow([&] { getter(bar0_get); }).fatal("Failed to retrieve object");
        expectNoThrow([&] { getter(bar1_get); }).fatal("Failed to retrieve object");

        // Compare objects.
        compareEQ(bar0.id, bar0_get.id);
        compareEQ(bar0.a.get(), bar0_get.a.get());
        compareEQ(bar0.b.get(), bar0_get.b.get());
        compareEQ(bar1.id, bar1_get.id);
        compareEQ(bar1.a.get(), bar1_get.a.get());
        compareEQ(bar1.b.get(), bar1_get.b.get());
    }
}
