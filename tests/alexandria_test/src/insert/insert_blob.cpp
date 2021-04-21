#include "alexandria_test/insert/insert_blob.h"

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

void InsertBlob::operator()()
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

    // Get tables.
    sql::ext::TypedTable<int64_t, Baz> fooTable(library->getDatabase().getTable(fooType.getName()));
    sql::ext::TypedTable<int64_t, std::vector<Baz>, std::vector<float>> barTable(
      library->getDatabase().getTable(barType.getName()));

    // Create object handlers.
    auto fooHandler = library->createObjectHandler<&Foo::id, &Foo::a>(fooType.getName());
    auto barHandler = library->createObjectHandler<&Bar::id, &Bar::a, &Bar::b>(barType.getName());

    // Insert Foo.
    {
        // Create objects.
        Foo foo0;
        foo0.a.get().x = 3.5f;
        foo0.a.get().y = 12;
        Foo foo1;
        foo1.a.get().x = -0.5f;
        foo1.a.get().y = -10;

        // Try to insert.
        expectNoThrow([&] { fooHandler.insert(foo0); }).fatal("Failed to insert object");
        expectNoThrow([&] { fooHandler.insert(foo1); }).fatal("Failed to insert object");

        // Check assigned IDs.
        compareEQ(foo0.id, static_cast<int64_t>(1));
        compareEQ(foo1.id, static_cast<int64_t>(2));

        // Select inserted object using sql.
        auto foo0_get = fooTable.selectOne(fooTable.col<0>() == foo0.id, true)(false);
        auto foo1_get = fooTable.selectOne(fooTable.col<0>() == foo1.id, true)(false);

        // Compare objects.
        compareEQ(foo0.id, std::get<0>(foo0_get));
        compareEQ(foo0.a.get(), std::get<1>(foo0_get));
        compareEQ(foo1.id, std::get<0>(foo1_get));
        compareEQ(foo1.a.get(), std::get<1>(foo1_get));
    }

    // Insert Bar.
    {
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
        expectNoThrow([&] { barHandler.insert(bar0); }).fatal("Failed to insert object");
        expectNoThrow([&] { barHandler.insert(bar1); }).fatal("Failed to insert object");

        // Check assigned IDs.
        compareEQ(bar0.id, static_cast<int64_t>(1));
        compareEQ(bar1.id, static_cast<int64_t>(2));

        // Select inserted object using sql.
        auto bar0_get = barTable.selectOne(barTable.col<0>() == bar0.id, true)(false);
        auto bar1_get = barTable.selectOne(barTable.col<0>() == bar1.id, true)(false);

        // Compare objects.
        compareEQ(bar0.id, std::get<0>(bar0_get));
        compareEQ(bar0.a.get(), std::get<1>(bar0_get));
        compareEQ(bar0.b.get(), std::get<2>(bar0_get));
        compareEQ(bar1.id, std::get<0>(bar1_get));
        compareEQ(bar1.a.get(), std::get<1>(bar1_get));
        compareEQ(bar1.b.get(), std::get<2>(bar1_get));
    }
}
