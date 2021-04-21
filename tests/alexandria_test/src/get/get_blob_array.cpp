#include "alexandria_test/get/get_blob_array.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria/library.h"
#include "alexandria/member_types/blob_array.h"

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
        int64_t              id = 0;
        alex::BlobArray<Baz> a;
    };

    struct Bar
    {
        int64_t                             id = 0;
        alex::BlobArray<std::vector<Baz>>   a;
        alex::BlobArray<std::vector<float>> b;
    };
}  // namespace

void GetBlobArray::operator()()
{
    // Create type with 1 blob.
    auto& fooType = library->createType("Foo");
    fooType.createBlobArrayProperty("blob1");

    // Create type with 2 blobs.
    auto& barType = library->createType("Bar");
    barType.createBlobArrayProperty("blob1");
    barType.createBlobArrayProperty("blob2");

    // Commit types.
    expectNoThrow([this]() { library->commitTypes(); }).fatal("Failed to commit types");

    // Create object handlers.
    auto fooHandler = library->createObjectHandler<&Foo::id, &Foo::a>(fooType.getName());
    auto barHandler = library->createObjectHandler<&Bar::id, &Bar::a, &Bar::b>(barType.getName());

    // Retrieve Foo.
    {
        // Create and insert objects.
        Foo foo0;
        foo0.a.get().emplace_back(Baz{.x = 0.5f, .y = 10});
        foo0.a.get().emplace_back(Baz{.x = -2.0f, .y = -32});
        Foo foo1;
        foo1.a.get().emplace_back(Baz{.x = 4.5f, .y = 10000});
        expectNoThrow([&] { fooHandler.insert(foo0); }).fatal("Failed to insert object");
        expectNoThrow([&] { fooHandler.insert(foo1); }).fatal("Failed to insert object");

        // Try to retrieve objects.
        std::unique_ptr<Foo> foo0_get, foo1_get;
        expectNoThrow([&] { foo0_get = fooHandler.get(foo0.id); }).fatal("Failed to get object");
        expectNoThrow([&] { foo1_get = fooHandler.get(foo1.id); }).fatal("Failed to get object");

        // Compare objects.
        compareEQ(foo0.id, foo0_get->id);
        compareEQ(foo0.a.get(), foo0_get->a.get());
        compareEQ(foo1.id, foo1_get->id);
        compareEQ(foo1.a.get(), foo1_get->a.get());
    }

    // Retrieve Bar.
    {
        // Create and insert objects.
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
        expectNoThrow([&] { barHandler.insert(bar0); }).fatal("Failed to insert object");
        expectNoThrow([&] { barHandler.insert(bar1); }).fatal("Failed to insert object");

        // Try to retrieve objects.
        std::unique_ptr<Bar> bar0_get, bar1_get;
        expectNoThrow([&] { bar0_get = barHandler.get(bar0.id); }).fatal("Failed to get object");
        expectNoThrow([&] { bar1_get = barHandler.get(bar1.id); }).fatal("Failed to get object");

        // Compare objects.
        compareEQ(bar0.id, bar0_get->id);
        compareEQ(bar0.a.get(), bar0_get->a.get());
        compareEQ(bar0.b.get(), bar0_get->b.get());
        compareEQ(bar1.id, bar1_get->id);
        compareEQ(bar1.a.get(), bar1_get->a.get());
        compareEQ(bar1.b.get(), bar1_get->b.get());
    }
}
