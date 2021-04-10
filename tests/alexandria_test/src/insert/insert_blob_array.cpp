#include "alexandria_test/insert/insert_blob_array.h"

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

void InsertBlobArray::operator()()
{
    // Create all property types.
    auto& blobProp1 = library->createBlobProperty("blob1", true);
    auto& blobProp2 = library->createBlobProperty("blob2", true);

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
    sql::ext::TypedTable<int64_t>               fooTable(library->getDatabase().getTable(fooType.getName()));
    sql::ext::TypedTable<int64_t>               barTable(library->getDatabase().getTable(barType.getName()));
    sql::ext::TypedTable<int64_t, int64_t, Baz> fooBlob1Table(
      library->getDatabase().getTable(fooType.getName() + "_blob1"));
    sql::ext::TypedTable<int64_t, int64_t, std::vector<Baz>> barBlob1Table(
      library->getDatabase().getTable(barType.getName() + "_blob1"));
    sql::ext::TypedTable<int64_t, int64_t, std::vector<float>> barBlob2Table(
      library->getDatabase().getTable(barType.getName() + "_blob2"));

    // Create object handlers.
    auto fooHandler = library->createObjectHandler<&Foo::id, &Foo::a>(fooType.getName());
    auto barHandler = library->createObjectHandler<&Bar::id, &Bar::a, &Bar::b>(barType.getName());

    // Insert Foo.
    Foo foo0;
    foo0.a.get().emplace_back(Baz{.x = 0.5f, .y = 10});
    foo0.a.get().emplace_back(Baz{.x = -2.0f, .y = -32});
    Foo foo1;
    foo1.a.get().emplace_back(Baz{.x = 4.5f, .y = 10000});
    fooHandler.insert(foo0);
    fooHandler.insert(foo1);
    // Check assigned IDs.
    compareEQ(foo0.id, static_cast<int64_t>(1));
    compareEQ(foo1.id, static_cast<int64_t>(2));
    // Select inserted object using sql and compare.
    auto foo0_get = fooTable.selectOne(fooTable.col<0>() == foo0.id, true)(false);
    auto foo1_get = fooTable.selectOne(fooTable.col<0>() == foo1.id, true)(false);
    compareEQ(foo0.id, std::get<0>(foo0_get));
    compareEQ(foo1.id, std::get<0>(foo1_get));
    // Select blobs in separate table.
    auto                         idparam      = foo0.id;
    auto                         blobs_select = fooBlob1Table.select<2>(fooBlob1Table.col<1>() == &idparam, true);
    std::vector<std::tuple<Baz>> blobs_get(blobs_select.begin(), blobs_select.end());
    compareEQ(foo0.a.get()[0].x, std::get<0>(blobs_get[0]).x);
    compareEQ(foo0.a.get()[0].y, std::get<0>(blobs_get[0]).y);
    compareEQ(foo0.a.get()[1].x, std::get<0>(blobs_get[1]).x);
    compareEQ(foo0.a.get()[1].y, std::get<0>(blobs_get[1]).y);
    idparam = foo1.id;
    blobs_get.assign(blobs_select(true).begin(), blobs_select.end());
    compareEQ(foo1.a.get()[0].x, std::get<0>(blobs_get[0]).x);
    compareEQ(foo1.a.get()[0].y, std::get<0>(blobs_get[0]).y);

    // Insert Bar.
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
    barHandler.insert(bar0);
    barHandler.insert(bar1);
    // Check assigned IDs.
    compareEQ(bar0.id, static_cast<int64_t>(1));
    compareEQ(bar1.id, static_cast<int64_t>(2));
    // Select inserted object using sql and compare.
    auto bar0_get = barTable.selectOne(barTable.col<0>() == bar0.id, true)(false);
    auto bar1_get = barTable.selectOne(barTable.col<0>() == bar1.id, true)(false);
    compareEQ(bar0.id, std::get<0>(bar0_get));
    compareEQ(bar1.id, std::get<0>(bar1_get));
    // Select blobs in separate table.
    idparam            = bar0.id;
    auto blobs1_select = barBlob1Table.select<2>(barBlob1Table.col<1>() == &idparam, true);
    auto blobs2_select = barBlob2Table.select<2>(barBlob2Table.col<1>() == &idparam, true);
    std::vector<std::tuple<std::vector<Baz>>>   blobs1_get(blobs1_select.begin(), blobs1_select.end());
    std::vector<std::tuple<std::vector<float>>> blobs2_get(blobs2_select.begin(), blobs2_select.end());
    compareEQ(bar0.a.get()[0][0].x, std::get<0>(blobs1_get[0])[0].x);
    compareEQ(bar0.a.get()[0][0].y, std::get<0>(blobs1_get[0])[0].y);
    compareEQ(bar0.a.get()[0][1].x, std::get<0>(blobs1_get[0])[1].x);
    compareEQ(bar0.a.get()[0][1].y, std::get<0>(blobs1_get[0])[1].y);
    compareEQ(bar0.a.get()[1][0].x, std::get<0>(blobs1_get[1])[0].x);
    compareEQ(bar0.a.get()[1][0].y, std::get<0>(blobs1_get[1])[0].y);
    compareEQ(bar0.b.get()[0][0], std::get<0>(blobs2_get[0])[0]);
    compareEQ(bar0.b.get()[0][1], std::get<0>(blobs2_get[0])[1]);
    compareEQ(bar0.b.get()[0][2], std::get<0>(blobs2_get[0])[2]);
    compareEQ(bar0.b.get()[1][0], std::get<0>(blobs2_get[1])[0]);
    compareEQ(bar0.b.get()[1][1], std::get<0>(blobs2_get[1])[1]);
    compareEQ(bar0.b.get()[2][0], std::get<0>(blobs2_get[2])[0]);
    idparam = bar1.id;
    blobs1_get.assign(blobs1_select(true).begin(), blobs1_select.end());
    blobs2_get.assign(blobs2_select(true).begin(), blobs2_select.end());
    compareEQ(bar1.a.get()[0][0].x, std::get<0>(blobs1_get[0])[0].x);
    compareEQ(bar1.a.get()[0][0].y, std::get<0>(blobs1_get[0])[0].y);
    compareEQ(bar1.b.get()[0][0], std::get<0>(blobs2_get[0])[0]);
    compareEQ(bar1.b.get()[0][1], std::get<0>(blobs2_get[0])[1]);
    compareEQ(bar1.b.get()[0][2], std::get<0>(blobs2_get[0])[2]);
    compareEQ(bar1.b.get()[1].size(), std::get<0>(blobs2_get[1]).size());
}
