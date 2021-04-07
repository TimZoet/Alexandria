#include "alexandria_test/insert/insert_blob.h"

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
    // Create all property types.
    auto& blobProp1 = library->createBlobProperty("blobProp1", false);
    auto& blobProp2 = library->createBlobProperty("blobProp2", false);

    // Create type with 1 blob.
    auto& fooType = library->createType("Foo");
    fooType.addProperty(blobProp1);

    // Create type with 2 blobs.
    auto& barType = library->createType("Bar");
    barType.addProperty(blobProp1);
    barType.addProperty(blobProp2);

    // Commit types.
    expectNoThrow([this]() { library->commitTypes(); });

    // Get tables.
    sql::ext::TypedTable<int64_t, Baz> fooTable(library->getDatabase().getTable(fooType.getName()));
    sql::ext::TypedTable<int64_t, std::vector<Baz>, std::vector<float>> barTable(
      library->getDatabase().getTable(barType.getName()));

    // Create object handlers.
    auto fooHandler = library->createObjectHandler<&Foo::id, &Foo::a>(fooType.getName());
    auto barHandler = library->createObjectHandler<&Bar::id, &Bar::a, &Bar::b>(barType.getName());

    // Insert Foo.
    Foo foo0;
    foo0.a.get().x = 3.5f;
    foo0.a.get().y = 12;
    Foo foo1;
    foo1.a.get().x = -0.5f;
    foo1.a.get().y = -10;
    fooHandler.insert(foo0);
    fooHandler.insert(foo1);
    // Check assigned IDs.
    compareEQ(foo0.id, static_cast<int64_t>(1));
    compareEQ(foo1.id, static_cast<int64_t>(2));
    // Select inserted object using sql and compare.
    auto foo0_get = fooTable.selectOne(fooTable.col<0>() == foo0.id, true)(false);
    auto foo1_get = fooTable.selectOne(fooTable.col<0>() == foo1.id, true)(false);
    compareEQ(foo0.id, std::get<0>(foo0_get));
    compareEQ(foo0.a.get().x, std::get<1>(foo0_get).x);
    compareEQ(foo0.a.get().y, std::get<1>(foo0_get).y);
    compareEQ(foo1.id, std::get<0>(foo1_get));
    compareEQ(foo1.a.get().x, std::get<1>(foo1_get).x);
    compareEQ(foo1.a.get().y, std::get<1>(foo1_get).y);

    // TODO: Use vector comparison methods once supported.

    // Insert Bar.
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
    barHandler.insert(bar0);
    barHandler.insert(bar1);
    // Check assigned IDs.
    compareEQ(bar0.id, static_cast<int64_t>(1));
    compareEQ(bar1.id, static_cast<int64_t>(2));
    // Select inserted object using sql and compare.
    auto bar0_get = barTable.selectOne(barTable.col<0>() == bar0.id, true)(false);
    auto bar1_get = barTable.selectOne(barTable.col<0>() == bar1.id, true)(false);
    compareEQ(bar0.id, std::get<0>(bar0_get));
    compareEQ(bar0.a.get()[0].x, std::get<1>(bar0_get)[0].x);
    compareEQ(bar0.a.get()[0].y, std::get<1>(bar0_get)[0].y);
    compareEQ(bar0.a.get()[1].x, std::get<1>(bar0_get)[1].x);
    compareEQ(bar0.a.get()[1].y, std::get<1>(bar0_get)[1].y);
    compareEQ(bar0.b.get()[0], std::get<2>(bar0_get)[0]);
    compareEQ(bar0.b.get()[1], std::get<2>(bar0_get)[1]);
    compareEQ(bar1.id, std::get<0>(bar1_get));
    compareEQ(bar1.a.get()[0].x, std::get<1>(bar1_get)[0].x);
    compareEQ(bar1.a.get()[0].y, std::get<1>(bar1_get)[0].y);
    compareEQ(bar1.b.get()[0], std::get<2>(bar1_get)[0]);
    compareEQ(bar1.b.get()[1], std::get<2>(bar1_get)[1]);
    compareEQ(bar1.b.get()[2], std::get<2>(bar1_get)[2]);
}
