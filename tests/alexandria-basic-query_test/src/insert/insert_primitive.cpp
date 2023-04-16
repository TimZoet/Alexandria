#include "alexandria-basic-query_test/insert/insert_primitive.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria-core/type_descriptor.h"
#include "alexandria-basic-query/insert_query.h"

namespace
{
    struct Foo
    {
        alex::InstanceId id;
        float            a = 0;
        double           b = 0;

        Foo(const float aa, const double bb) : a(aa), b(bb) {}

        Foo(const std::string& iid, const float aa, const double bb) : id(iid), a(aa), b(bb) {}
    };

    struct Bar
    {
        alex::InstanceId id;
        int32_t          a = 0;
        int64_t          b = 0;
        uint32_t         c = 0;
        uint64_t         d = 0;

        Bar(const int32_t aa, const int64_t bb, const uint32_t cc, const uint64_t dd) : a(aa), b(bb), c(cc), d(dd) {}

        Bar(const std::string& iid, const int32_t aa, const int64_t bb, const uint32_t cc, const uint64_t dd) :
            id(iid), a(aa), b(bb), c(cc), d(dd)
        {
        }
    };

    using FooDescriptor = alex::
      GenerateTypeDescriptor<alex::Member<"id", &Foo::id>, alex::Member<"a", &Foo::a>, alex::Member<"b", &Foo::b>>;

    using BarDescriptor = alex::GenerateTypeDescriptor<alex::Member<"id", &Bar::id>,
                                                       alex::Member<"a", &Bar::a>,
                                                       alex::Member<"b", &Bar::b>,
                                                       alex::Member<"c", &Bar::c>,
                                                       alex::Member<"d", &Bar::d>>;
}  // namespace

void InsertPrimitive::operator()()
{
    expectNoThrow([&] {
        alex::TypeLayout fooLayout;
        fooLayout.createPrimitiveProperty("prop0", alex::DataType::Float);
        fooLayout.createPrimitiveProperty("prop1", alex::DataType::Double);
        fooLayout.commit(*nameSpace, "foo");

        alex::TypeLayout barLayout;
        barLayout.createPrimitiveProperty("prop0", alex::DataType::Int32);
        barLayout.createPrimitiveProperty("prop1", alex::DataType::Int64);
        barLayout.createPrimitiveProperty("prop2", alex::DataType::Uint32);
        barLayout.createPrimitiveProperty("prop3", alex::DataType::Uint64);
        barLayout.commit(*nameSpace, "bar");
    }).fatal("Failed to commit types");

    auto& fooType = nameSpace->getType("foo");
    auto& barType = nameSpace->getType("bar");

    // Insert Foo.
    {
        const sql::TypedTable<sql::row_id, std::string, float, double> table(fooType.getInstanceTable());
        auto inserter = alex::InsertQuery(FooDescriptor(fooType));

        // Create objects.
        Foo foo0(0.5f, 1.5);
        Foo foo1(-0.5f, -1.5);

        // Try to insert.
        expectNoThrow([&] { inserter(foo0); }).fatal("Failed to insert object");
        expectNoThrow([&] { inserter(foo1); }).fatal("Failed to insert object");

        // Check assigned IDs.
        compareTrue(foo0.id.valid());
        compareTrue(foo1.id.valid());

        // Select inserted object using sql and compare.
        std::string id;
        auto        stmt    = table.selectAs<Foo, 1, 2, 3>().where(like(table.col<1>(), &id)).compileOne();
        id                  = foo0.id.getAsString();
        const auto foo0_get = stmt.bind(sql::BindParameters::All)();
        id                  = foo1.id.getAsString();
        const auto foo1_get = stmt.bind(sql::BindParameters::All)();

        // Compare objects.
        compareEQ(foo0.id, foo0_get.id);
        compareEQ(foo0.a, foo0_get.a);
        compareEQ(foo0.b, foo0_get.b);
        compareEQ(foo1.id, foo1_get.id);
        compareEQ(foo1.a, foo1_get.a);
        compareEQ(foo1.b, foo1_get.b);
    }

    // Insert Bar.
    {
        const sql::TypedTable<sql::row_id, std::string, int32_t, int64_t, uint32_t, uint64_t> table(
          barType.getInstanceTable());
        auto inserter = alex::InsertQuery(BarDescriptor(barType));

        // Create objects.
        Bar bar0(1, 2, 3, 4);
        Bar bar1(-1, -2, 123456, 1234567);

        // Try to insert.
        expectNoThrow([&] { inserter(bar0); }).fatal("Failed to insert object");
        expectNoThrow([&] { inserter(bar1); }).fatal("Failed to insert object");

        // Check assigned IDs.
        compareTrue(bar0.id.valid());
        compareTrue(bar1.id.valid());

        // Select inserted object using sql and compare.
        std::string id;
        auto        stmt    = table.selectAs<Bar, 1, 2, 3, 4, 5>().where(like(table.col<1>(), &id)).compileOne();
        id                  = bar0.id.getAsString();
        const auto bar0_get = stmt.bind(sql::BindParameters::All)();
        id                  = bar1.id.getAsString();
        const auto bar1_get = stmt.bind(sql::BindParameters::All)();

        // Compare objects.
        compareEQ(bar0.id, bar0_get.id);
        compareEQ(bar0.a, bar0_get.a);
        compareEQ(bar0.b, bar0_get.b);
        compareEQ(bar0.c, bar0_get.c);
        compareEQ(bar0.d, bar0_get.d);
        compareEQ(bar1.id, bar1_get.id);
        compareEQ(bar1.a, bar1_get.a);
        compareEQ(bar1.b, bar1_get.b);
        compareEQ(bar1.c, bar1_get.c);
        compareEQ(bar1.d, bar1_get.d);
    }
}
