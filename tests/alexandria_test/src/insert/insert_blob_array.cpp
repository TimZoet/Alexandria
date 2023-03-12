#include "alexandria_test/insert/insert_blob_array.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria/core/library.h"
#include "alexandria/core/type_descriptor.h"
#include "alexandria/queries/insert_query.h"

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

void InsertBlobArray::operator()()
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

    // Insert Foo.
    {
        const sql::TypedTable<sql::row_id, std::string, Baz> arrayTable(
          library->getDatabase().getTable("main_foo_blob1"));

        auto inserter = alex::InsertQuery(FooDescriptor(fooType));

        // Create objects.
        Foo foo0;
        foo0.a.get().emplace_back(Baz{.x = 0.5f, .y = 10});
        foo0.a.get().emplace_back(Baz{.x = -2.0f, .y = -32});
        Foo foo1;
        foo1.a.get().emplace_back(Baz{.x = 4.5f, .y = 10000});
        Foo foo2;

        // Try to insert.
        expectNoThrow([&] { inserter(foo0); }).fatal("Failed to insert object");
        expectNoThrow([&] { inserter(foo1); }).fatal("Failed to insert object");
        expectNoThrow([&] { inserter(foo2); }).fatal("Failed to insert object");

        // Check assigned IDs.
        compareTrue(foo0.id.valid());
        compareTrue(foo1.id.valid());
        compareTrue(foo2.id.valid());

        // Select blobs in array table.
        std::string id;
        auto        stmt = arrayTable.selectAs<Baz, 2>()
                      .where(like(arrayTable.col<1>(), &id))
                      .orderBy(ascending(arrayTable.col<0>()))
                      .compile();
        id = foo0.id.getAsString();
        stmt.bind(sql::BindParameters::All);
        std::vector<Baz> blobs(stmt.begin(), stmt.end());
        compareEQ(foo0.a.get(), blobs);
        id = foo1.id.getAsString();
        stmt.bind(sql::BindParameters::All);
        blobs.assign(stmt.begin(), stmt.end());
        compareEQ(foo1.a.get(), blobs);
        id = foo2.id.getAsString();
        stmt.bind(sql::BindParameters::All);
        blobs.assign(stmt.begin(), stmt.end());
        compareEQ(foo2.a.get(), blobs);
    }

    // Insert Bar.
    {
        const sql::TypedTable<sql::row_id, std::string, std::vector<Baz>> array0Table(
          library->getDatabase().getTable("main_bar_blob1"));
        const sql::TypedTable<sql::row_id, std::string, std::vector<float>> array1Table(
          library->getDatabase().getTable("main_bar_blob2"));

        auto inserter = alex::InsertQuery(BarDescriptor(barType));

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

        // Check assigned IDs.
        compareTrue(bar0.id.valid());
        compareTrue(bar1.id.valid());
        compareTrue(bar2.id.valid());

        // Select blobs in array table.
        std::string id;
        auto        stmt0 = array0Table.selectAs<std::vector<Baz>, 2>()
                       .where(like(array0Table.col<1>(), &id))
                       .orderBy(ascending(array0Table.col<0>()))
                       .compile();
        auto stmt1 = array1Table.selectAs<std::vector<float>, 2>()
                       .where(like(array1Table.col<1>(), &id))
                       .orderBy(ascending(array1Table.col<0>()))
                       .compile();
        id = bar0.id.getAsString();
        stmt0.bind(sql::BindParameters::All);
        stmt1.bind(sql::BindParameters::All);
        std::vector<std::vector<Baz>>   blobs(stmt0.begin(), stmt0.end());
        std::vector<std::vector<float>> floats(stmt1.begin(), stmt1.end());
        compareEQ(bar0.a.get(), blobs);
        compareEQ(bar0.b.get(), floats);
        id = bar1.id.getAsString();
        stmt0.bind(sql::BindParameters::All);
        stmt1.bind(sql::BindParameters::All);
        blobs.assign(stmt0.begin(), stmt0.end());
        floats.assign(stmt1.begin(), stmt1.end());
        compareEQ(bar1.a.get(), blobs);
        compareEQ(bar1.b.get(), floats);
        id = bar2.id.getAsString();
        stmt0.bind(sql::BindParameters::All);
        stmt1.bind(sql::BindParameters::All);
        blobs.assign(stmt0.begin(), stmt0.end());
        floats.assign(stmt1.begin(), stmt1.end());
        compareEQ(bar2.a.get(), blobs);
        compareEQ(bar2.b.get(), floats);
    }
}
