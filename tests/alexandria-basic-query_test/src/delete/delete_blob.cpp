#include "alexandria-basic-query_test/delete/delete_blob.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria-core/type_descriptor.h"
#include "alexandria-basic-query/delete_query.h"
#include "alexandria-basic-query/insert_query.h"

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

    using FooDescriptor = alex::GenerateTypeDescriptor<alex::Member<"id", &Foo::id>, alex::Member<"a", &Foo::a>>;

    using BarDescriptor = alex::
      GenerateTypeDescriptor<alex::Member<"id", &Bar::id>, alex::Member<"a", &Bar::a>, alex::Member<"b", &Bar::b>>;
}  // namespace

void DeleteBlob::operator()()
{
    expectNoThrow([&] {
        alex::TypeLayout fooLayout;
        fooLayout.createBlobProperty("prop0");
        fooLayout.commit(*nameSpace, "foo");

        alex::TypeLayout barLayout;
        barLayout.createBlobProperty("prop0");
        barLayout.createBlobProperty("prop1");
        barLayout.commit(*nameSpace, "bar");
    }).fatal("Failed to commit types");

    auto& fooType = nameSpace->getType("foo");
    auto& barType = nameSpace->getType("bar");

    // Delete Foo.
    {
        const sql::TypedTable<sql::row_id, std::string, Baz> table(fooType.getInstanceTable());
        auto                                                 inserter = alex::InsertQuery(FooDescriptor(fooType));
        auto                                                 deleter  = alex::DeleteQuery(FooDescriptor(fooType));

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

        // Verify existence of objects before and after delete.
        std::string id;
        auto        stmt = table.count().where(like(table.col<1>(), &id)).compile();
        id               = foo0.id.getAsString();
        compareEQ(1, stmt.bind(sql::BindParameters::All)());
        expectNoThrow([&] { deleter(foo0); });
        compareEQ(0, stmt.bind(sql::BindParameters::All)());
        id = foo1.id.getAsString();
        compareEQ(1, stmt.bind(sql::BindParameters::All)());
        expectNoThrow([&] { deleter(foo1); });
        compareEQ(0, stmt.bind(sql::BindParameters::All)());
    }

    // Delete Bar.
    {
        const sql::TypedTable<sql::row_id, std::string, std::vector<Baz>, std::vector<float>> table(
          barType.getInstanceTable());
        auto inserter = alex::InsertQuery(BarDescriptor(barType));
        auto deleter  = alex::DeleteQuery(BarDescriptor(barType));

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
        Bar bar2;

        // Try to insert.
        expectNoThrow([&] { inserter(bar0); }).fatal("Failed to insert object");
        expectNoThrow([&] { inserter(bar1); }).fatal("Failed to insert object");
        expectNoThrow([&] { inserter(bar2); }).fatal("Failed to insert object");

        // Verify existence of objects before and after delete.
        std::string id;
        auto        stmt = table.count().where(like(table.col<1>(), &id)).compile();
        id               = bar0.id.getAsString();
        compareEQ(1, stmt.bind(sql::BindParameters::All)());
        expectNoThrow([&] { deleter(bar0); });
        compareEQ(0, stmt.bind(sql::BindParameters::All)());
        id = bar1.id.getAsString();
        compareEQ(1, stmt.bind(sql::BindParameters::All)());
        expectNoThrow([&] { deleter(bar1); });
        compareEQ(0, stmt.bind(sql::BindParameters::All)());
        id = bar2.id.getAsString();
        compareEQ(1, stmt.bind(sql::BindParameters::All)());
        expectNoThrow([&] { deleter(bar2); });
        compareEQ(0, stmt.bind(sql::BindParameters::All)());
    }
}
