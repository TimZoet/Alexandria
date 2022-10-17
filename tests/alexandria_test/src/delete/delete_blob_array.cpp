#include "alexandria_test/delete/delete_blob_array.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria/library.h"
#include "alexandria/member_types/blob_array.h"
#include "alexandria/member_types/member.h"

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
}  // namespace

void DeleteBlobArray::operator()()
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

    // Get tables.
    sql::TypedTable<int64_t>               fooTable(library->getDatabase().getTable(fooType.getName()));
    sql::TypedTable<int64_t>               barTable(library->getDatabase().getTable(barType.getName()));
    sql::TypedTable<int64_t, int64_t, Baz> fooBlob1Table(
      library->getDatabase().getTable(fooType.getName() + "_blob1"));
    sql::TypedTable<int64_t, int64_t, std::vector<Baz>> barBlob1Table(
      library->getDatabase().getTable(barType.getName() + "_blob1"));
    sql::TypedTable<int64_t, int64_t, std::vector<float>> barBlob2Table(
      library->getDatabase().getTable(barType.getName() + "_blob2"));

    // Create object handlers.
    auto fooHandler = library->createObjectHandler<alex::Member<&Foo::id>, alex::Member<&Foo::a>>(fooType.getName());
    auto barHandler =
      library->createObjectHandler<alex::Member<&Bar::id>, alex::Member<&Bar::a>, alex::Member<&Bar::b>>(
        barType.getName());

    // Delete Foo.
    {
        // Create and insert objects.
        Foo foo0;
        foo0.a.get().emplace_back(Baz{.x = 0.5f, .y = 10});
        foo0.a.get().emplace_back(Baz{.x = -2.0f, .y = -32});
        Foo foo1;
        foo1.a.get().emplace_back(Baz{.x = 4.5f, .y = 10000});
        expectNoThrow([&] { fooHandler->insert(foo0); }).fatal("Failed to insert object");
        expectNoThrow([&] { fooHandler->insert(foo1); }).fatal("Failed to insert object");

        // Delete objects one by one and check tables.
        compareEQ(static_cast<size_t>(2), fooTable.countAll()(true));
        compareEQ(static_cast<size_t>(2 + 1), fooBlob1Table.countAll()(true));
        expectNoThrow([&] { fooHandler->del(foo0.id); });
        compareEQ(static_cast<size_t>(1), fooTable.countAll()(true));
        compareEQ(static_cast<size_t>(1), fooBlob1Table.countAll()(true));
        expectNoThrow([&] { fooHandler->del(foo1.id); });
        compareEQ(static_cast<size_t>(0), fooTable.countAll()(true));
        compareEQ(static_cast<size_t>(0), fooBlob1Table.countAll()(true));
    }

    // Delete Bar.
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
        expectNoThrow([&] { barHandler->insert(bar0); }).fatal("Failed to insert object");
        expectNoThrow([&] { barHandler->insert(bar1); }).fatal("Failed to insert object");

        // Delete objects one by one and check tables.
        compareEQ(static_cast<size_t>(2), barTable.countAll()(true));
        compareEQ(static_cast<size_t>(2 + 1), barBlob1Table.countAll()(true));
        compareEQ(static_cast<size_t>(3 + 2), barBlob2Table.countAll()(true));
        expectNoThrow([&] { barHandler->del(bar0.id); });
        compareEQ(static_cast<size_t>(1), barTable.countAll()(true));
        compareEQ(static_cast<size_t>(1), barBlob1Table.countAll()(true));
        compareEQ(static_cast<size_t>(2), barBlob2Table.countAll()(true));
        expectNoThrow([&] { barHandler->del(bar1.id); });
        compareEQ(static_cast<size_t>(0), barTable.countAll()(true));
        compareEQ(static_cast<size_t>(0), barBlob1Table.countAll()(true));
        compareEQ(static_cast<size_t>(0), barBlob2Table.countAll()(true));
    }
}
