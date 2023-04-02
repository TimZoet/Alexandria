#include "alexandria_test/delete/delete_blob_array.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria-core/library.h"
#include "alexandria-core/type_descriptor.h"
#include "alexandria-query/delete_query.h"
#include "alexandria-query/insert_query.h"

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

    using FooDescriptor = alex::GenerateTypeDescriptor<alex::Member<"id", &Foo::id>, alex::Member<"a", &Foo::a>>;

    using BarDescriptor = alex::
      GenerateTypeDescriptor<alex::Member<"id", &Bar::id>, alex::Member<"a", &Bar::a>, alex::Member<"b", &Bar::b>>;
}  // namespace

void DeleteBlobArray::operator()()
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

    // Delete Foo.
    {
        const sql::TypedTable<sql::row_id, std::string, Baz> arrayTable(
          library->getDatabase().getTable("main_foo_blob1"));

        auto inserter = alex::InsertQuery(FooDescriptor(fooType));
        auto deleter  = alex::DeleteQuery(FooDescriptor(fooType));

        // Create objects.
        Foo foo0;
        foo0.a.get().emplace_back(Baz{.x = 0.5f, .y = 10});
        foo0.a.get().emplace_back(Baz{.x = -2.0f, .y = -32});
        Foo foo1;
        foo1.a.get().emplace_back(Baz{.x = 4.5f, .y = 10000});

        // Try to insert.
        expectNoThrow([&] { inserter(foo0); }).fatal("Failed to insert object");
        expectNoThrow([&] { inserter(foo1); }).fatal("Failed to insert object");

        // Verify existence of objects before and after delete.
        std::string id;
        auto        stmt = arrayTable.count().where(like(arrayTable.col<1>(), &id)).compile();
        id               = foo0.id.getAsString();
        compareEQ(2, stmt.bind(sql::BindParameters::All)());
        expectNoThrow([&] { deleter(foo0); });
        compareEQ(0, stmt.bind(sql::BindParameters::All)());
        id = foo1.id.getAsString();
        compareEQ(1, stmt.bind(sql::BindParameters::All)());
        expectNoThrow([&] { deleter(foo1); });
        compareEQ(0, stmt.bind(sql::BindParameters::All)());
    }

    // Delete Bar.
    {
        const sql::TypedTable<sql::row_id, std::string, std::vector<Baz>> array0Table(
          library->getDatabase().getTable("main_bar_blob1"));
        const sql::TypedTable<sql::row_id, std::string, std::vector<float>> array1Table(
          library->getDatabase().getTable("main_bar_blob2"));

        auto inserter = alex::InsertQuery(BarDescriptor(barType));
        auto deleter  = alex::DeleteQuery(BarDescriptor(barType));

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
        Bar bar2;

        // Try to insert.
        expectNoThrow([&] { inserter(bar0); }).fatal("Failed to insert object");
        expectNoThrow([&] { inserter(bar1); }).fatal("Failed to insert object");
        expectNoThrow([&] { inserter(bar2); }).fatal("Failed to insert object");

        // Verify existence of objects before and after delete.
        std::string id;
        auto        stmt0 = array0Table.count().where(like(array0Table.col<1>(), &id)).compile();
        auto        stmt1 = array1Table.count().where(like(array1Table.col<1>(), &id)).compile();
        id                = bar0.id.getAsString();
        stmt0.bind(sql::BindParameters::All);
        stmt1.bind(sql::BindParameters::All);
        compareEQ(2, stmt0());
        compareEQ(3, stmt1());
        expectNoThrow([&] { deleter(bar0); });
        compareEQ(0, stmt0());
        compareEQ(0, stmt1());
        id = bar1.id.getAsString();
        stmt0.bind(sql::BindParameters::All);
        stmt1.bind(sql::BindParameters::All);
        compareEQ(1, stmt0());
        compareEQ(2, stmt1());
        expectNoThrow([&] { deleter(bar1); });
        compareEQ(0, stmt0());
        compareEQ(0, stmt1());
        id = bar2.id.getAsString();
        stmt0.bind(sql::BindParameters::All);
        stmt1.bind(sql::BindParameters::All);
        compareEQ(0, stmt0());
        compareEQ(0, stmt1());
        expectNoThrow([&] { deleter(bar2); });
        compareEQ(0, stmt0());
        compareEQ(0, stmt1());
    }
}
