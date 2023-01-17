#include "alexandria_test/delete/delete_primitive.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria/core/type_descriptor.h"
#include "alexandria/queries/delete_query.h"
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

void DeletePrimitive::operator()()
{
    // Create type with floats.
    auto& fooType = nameSpace->createType("foo");
    fooType.createPrimitiveProperty("floatprop", alex::DataType::Float);
    fooType.createPrimitiveProperty("doubleprop", alex::DataType::Double);

    // Create type with integers.
    auto& barType = nameSpace->createType("bar");
    barType.createPrimitiveProperty("int32prop", alex::DataType::Int32);
    barType.createPrimitiveProperty("int64prop", alex::DataType::Int64);
    barType.createPrimitiveProperty("uint32prop", alex::DataType::Uint32);
    barType.createPrimitiveProperty("uint64prop", alex::DataType::Uint64);

    // Commit types.
    expectNoThrow([&] {
        fooType.commit();
        barType.commit();
    }).fatal("Failed to commit types");

    // Delete Foo.
    {
        const sql::TypedTable<sql::row_id, std::string, float, double> table(fooType.getInstanceTable());
        auto inserter = alex::InsertQuery(FooDescriptor(fooType));
        auto deleter  = alex::DeleteQuery(FooDescriptor(fooType));

        // Create objects.
        Foo foo0{.a = 0.5f, .b = 1.5};
        Foo foo1{.a = -0.5f, .b = -1.5};

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
        const sql::TypedTable<sql::row_id, std::string, int32_t, int64_t, uint32_t, uint64_t> table(
          barType.getInstanceTable());
        auto inserter = alex::InsertQuery(BarDescriptor(barType));
        auto deleter  = alex::DeleteQuery(BarDescriptor(barType));

        // Create objects.
        Bar bar0{.a = 1, .b = 2, .c = 3, .d = 4};
        Bar bar1{.a = -1, .b = -2, .c = 123456, .d = 1234567};

        // Try to insert.
        expectNoThrow([&] { inserter(bar0); }).fatal("Failed to insert object");
        expectNoThrow([&] { inserter(bar1); }).fatal("Failed to insert object");

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
    }
}
