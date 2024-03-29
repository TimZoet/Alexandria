#include "alexandria-basic-query_test/insert/insert_blob.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria-core/type_descriptor.h"
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

void InsertBlob::operator()()
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

    // Insert Foo.
    {
        const sql::TypedTable<sql::row_id, std::string, Baz> table(fooType.getInstanceTable());
        auto                                                 inserter = alex::InsertQuery(FooDescriptor(fooType));

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

        // Check assigned IDs.
        compareTrue(foo0.id.valid());
        compareTrue(foo1.id.valid());

        // Select inserted object using sql and compare.
        std::string id;
        auto        stmt    = table.select<1, 2>().where(like(table.col<1>(), &id)).compileOne();
        id                  = foo0.id.getAsString();
        const auto foo0_get = stmt.bind(sql::BindParameters::All)();
        id                  = foo1.id.getAsString();
        const auto foo1_get = stmt.bind(sql::BindParameters::All)();

        // Compare objects.
        compareEQ(foo0.id, alex::InstanceId(std::get<0>(foo0_get)));
        compareEQ(foo0.a.get(), std::get<1>(foo0_get));
        compareEQ(foo1.id, alex::InstanceId(std::get<0>(foo1_get)));
        compareEQ(foo1.a.get(), std::get<1>(foo1_get));
    }

    // Insert Bar.
    {
        const sql::TypedTable<sql::row_id, std::string, std::vector<Baz>, std::vector<float>> table(
          barType.getInstanceTable());
        auto inserter = alex::InsertQuery(BarDescriptor(barType));

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

        // Check assigned IDs.
        compareTrue(bar0.id.valid());
        compareTrue(bar1.id.valid());
        compareTrue(bar2.id.valid());

        // Select inserted object using sql and compare.
        std::string id;
        auto        stmt    = table.select<1, 2, 3>().where(like(table.col<1>(), &id)).compileOne();
        id                  = bar0.id.getAsString();
        const auto bar0_get = stmt.bind(sql::BindParameters::All)();
        id                  = bar1.id.getAsString();
        const auto bar1_get = stmt.bind(sql::BindParameters::All)();
        id                  = bar2.id.getAsString();
        const auto bar2_get = stmt.bind(sql::BindParameters::All)();

        // Compare objects.
        compareEQ(bar0.id, alex::InstanceId(std::get<0>(bar0_get)));
        compareEQ(bar0.a.get(), std::get<1>(bar0_get));
        compareEQ(bar0.b.get(), std::get<2>(bar0_get));
        compareEQ(bar1.id, alex::InstanceId(std::get<0>(bar1_get)));
        compareEQ(bar1.a.get(), std::get<1>(bar1_get));
        compareEQ(bar1.b.get(), std::get<2>(bar1_get));
        compareEQ(bar2.id, alex::InstanceId(std::get<0>(bar2_get)));
        compareEQ(bar2.a.get(), std::get<1>(bar2_get));
        compareEQ(bar2.b.get(), std::get<2>(bar2_get));
    }
}
