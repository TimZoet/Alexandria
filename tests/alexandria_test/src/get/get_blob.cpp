#include "alexandria_test/get/get_blob.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <ostream>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria/library.h"
#include "alexandria/member_types/blob.h"

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
        int64_t         id = 0;
        alex::Blob<Baz> a;
    };

    struct Bar
    {
        int64_t                        id = 0;
        alex::Blob<std::vector<Baz>>   a;
        alex::Blob<std::vector<float>> b;
    };
}  // namespace

void GetBlob::operator()()
{
    // Create type with 1 blob.
    auto& fooType = library->createType("Foo");
    fooType.createBlobProperty("blobProp1");

    // Create type with 2 blobs.
    auto& barType = library->createType("Bar");
    barType.createBlobProperty("blobProp1");
    barType.createBlobProperty("blobProp2");

    // Commit types.
    expectNoThrow([this]() { library->commitTypes(); }).fatal("Failed to commit types");

    // Create object handlers.
    auto fooHandler = library->createObjectHandler<&Foo::id, &Foo::a>(fooType.getName());
    auto barHandler = library->createObjectHandler<&Bar::id, &Bar::a, &Bar::b>(barType.getName());

    // Retrieve Foo.
    {
        // Create and insert objects.
        Foo foo0;
        foo0.a.get().x = 3.5f;
        foo0.a.get().y = 12;
        Foo foo1;
        foo1.a.get().x = -0.5f;
        foo1.a.get().y = -10;
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
        bar0.a.get().push_back(Baz{.x = 1.0f, .y = 2});
        bar0.a.get().push_back(Baz{.x = -1.0f, .y = -13});
        bar0.b.get().push_back(33.0f);
        bar0.b.get().push_back(42.0f);
        Bar bar1;
        bar1.a.get().push_back(Baz{.x = 44.0f, .y = -11111});
        bar1.b.get().push_back(-10.0f);
        bar1.b.get().push_back(-20.0f);
        bar1.b.get().push_back(-30.0f);
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
