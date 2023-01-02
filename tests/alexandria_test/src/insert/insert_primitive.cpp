#include "alexandria_test/insert/insert_primitive.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria/core/library.h"
#include "alexandria/queries/insert_query.h"

namespace
{
    struct Foo
    {
        alex::InstanceId id;
        float            a = 0;
        double           b = 0;
    };

    struct Bar
    {
        alex::InstanceId id;
        int32_t          a = 0;
        int64_t          b = 0;
        uint32_t         c = 0;
        uint64_t         d = 0;
    };

    using FooDescriptor =
      alex::GenerateTypeDescriptor<alex::Member<&Foo::id>, alex::Member<&Foo::a>, alex::Member<&Foo::b>>;

    using BarDescriptor = alex::GenerateTypeDescriptor<alex::Member<&Bar::id>,
                                                       alex::Member<&Bar::a>,
                                                       alex::Member<&Bar::b>,
                                                       alex::Member<&Bar::c>,
                                                       alex::Member<&Bar::d>>;
}  // namespace

void InsertPrimitive::operator()()
{
    // Create type with floats.
    auto& fooType = nameSpace->createType("Foo");
    fooType.createPrimitiveProperty("floatProp", alex::DataType::Float);
    fooType.createPrimitiveProperty("doubleProp", alex::DataType::Double);

    // Create type with integers.
    auto& barType = nameSpace->createType("Bar");
    barType.createPrimitiveProperty("int32Prop", alex::DataType::Int32);
    barType.createPrimitiveProperty("int64Prop", alex::DataType::Int64);
    barType.createPrimitiveProperty("uint32Prop", alex::DataType::Uint32);
    barType.createPrimitiveProperty("uint64Prop", alex::DataType::Uint64);

    // Commit types.
    expectNoThrow([&] {
        fooType.commit();
        barType.commit();
    }).fatal("Failed to commit types");

    // Insert Foo.
    {
        const sql::TypedTable<std::string, float, double> table(fooType.getInstanceTable());
        auto                                              inserter = alex::InsertQuery(FooDescriptor(fooType));

        // Create objects.
        Foo foo0{.a = 0.5f, .b = 1.5};
        Foo foo1{.a = -0.5f, .b = -1.5};

        // Try to insert.
        expectNoThrow([&] { inserter(foo0); }).fatal("Failed to insert object");
        expectNoThrow([&] { inserter(foo1); }).fatal("Failed to insert object");

        // Check assigned IDs.
        compareTrue(foo0.id.valid());
        compareTrue(foo1.id.valid());

        // Select inserted object using sql and compare.
        std::string id;
        auto        stmt    = table.selectAs<Foo>().where(like(table.col<0>(), &id)).compileOne();
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
        const sql::TypedTable<std::string, int32_t, int64_t, uint32_t, uint64_t> table(barType.getInstanceTable());
        auto inserter = alex::InsertQuery(BarDescriptor(barType));

        // Create objects.
        Bar bar0{.a = 1, .b = 2, .c = 3, .d = 4};
        Bar bar1{.a = -1, .b = -2, .c = 123456, .d = 1234567};

        // Try to insert.
        expectNoThrow([&] { inserter(bar0); }).fatal("Failed to insert object");
        expectNoThrow([&] { inserter(bar1); }).fatal("Failed to insert object");

        // Check assigned IDs.
        compareTrue(bar0.id.valid());
        compareTrue(bar1.id.valid());

        // Select inserted object using sql and compare.
        std::string id;
        auto        stmt    = table.selectAs<Bar>().where(like(table.col<0>(), &id)).compileOne();
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
