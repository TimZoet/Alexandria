#include "alexandria_test/delete/delete_primitive_array.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria-core/library.h"
#include "alexandria-core/type_descriptor.h"
#include "alexandria-query/delete_query.h"
#include "alexandria-query/insert_query.h"

namespace
{
    struct Foo
    {
        alex::InstanceId            id;
        alex::PrimitiveArray<float> floats;

        Foo() = default;

        Foo(const alex::InstanceId iid, std::vector<float> ffloats) : id(iid) { floats.get() = std::move(ffloats); }
    };

    struct Bar
    {
        alex::InstanceId              id;
        alex::PrimitiveArray<int32_t> ints;

        Bar() = default;

        Bar(const alex::InstanceId iid, std::vector<int32_t> iints) : id(iid) { ints.get() = std::move(iints); }
    };

    struct Baz
    {
        alex::InstanceId               id;
        alex::PrimitiveArray<uint64_t> uints;
        alex::PrimitiveArray<double>   doubles;

        Baz() = default;

        Baz(const alex::InstanceId iid, std::vector<uint64_t> iints, std::vector<double> ffloats) : id(iid)
        {
            uints.get()   = std::move(iints);
            doubles.get() = std::move(ffloats);
        }
    };

    using FooDescriptor =
      alex::GenerateTypeDescriptor<alex::Member<"id", &Foo::id>, alex::Member<"floats", &Foo::floats>>;

    using BarDescriptor = alex::GenerateTypeDescriptor<alex::Member<"id", &Bar::id>, alex::Member<"ints", &Bar::ints>>;

    using BazDescriptor = alex::GenerateTypeDescriptor<alex::Member<"id", &Baz::id>,
                                                       alex::Member<"uints", &Baz::uints>,
                                                       alex::Member<"doubles", &Baz::doubles>>;
}  // namespace

void DeletePrimitiveArray::operator()()
{
    // Create type with floats.
    auto& fooType = nameSpace->createType("foo");
    fooType.createPrimitiveArrayProperty("floats", alex::DataType::Float);

    // Create type with integers.
    auto& barType = nameSpace->createType("bar");
    barType.createPrimitiveArrayProperty("ints", alex::DataType::Int32);

    // Create type with floats and integers.
    auto& bazType = nameSpace->createType("baz");
    bazType.createPrimitiveArrayProperty("uints", alex::DataType::Uint64);
    bazType.createPrimitiveArrayProperty("doubles", alex::DataType::Double);

    // Commit types.
    expectNoThrow([&] {
        fooType.commit();
        barType.commit();
        bazType.commit();
    }).fatal("Failed to commit types");

    // Delete Foo.
    {
        const sql::TypedTable<sql::row_id, std::string, float> arrayTable(
          library->getDatabase().getTable("main_foo_floats"));

        auto inserter = alex::InsertQuery(FooDescriptor(fooType));
        auto deleter  = alex::DeleteQuery(FooDescriptor(fooType));

        // Create objects.
        Foo foo0;
        foo0.floats.get().push_back(0.5f);
        foo0.floats.get().push_back(1.5f);
        Foo foo1;
        foo1.floats.get().push_back(-2.5f);
        foo1.floats.get().push_back(-3.5f);
        foo1.floats.get().push_back(-4.5f);

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
        compareEQ(3, stmt.bind(sql::BindParameters::All)());
        expectNoThrow([&] { deleter(foo1); });
        compareEQ(0, stmt.bind(sql::BindParameters::All)());
    }

    // Delete Bar.
    {
        const sql::TypedTable<sql::row_id, std::string, int32_t> arrayTable(
          library->getDatabase().getTable("main_bar_ints"));

        auto inserter = alex::InsertQuery(BarDescriptor(barType));
        auto deleter  = alex::DeleteQuery(BarDescriptor(barType));

        // Create objects.
        Bar bar0;
        bar0.ints.get().push_back(10);
        bar0.ints.get().push_back(100);
        Bar bar1;
        bar1.ints.get().push_back(-111);
        bar1.ints.get().push_back(-2222);
        bar1.ints.get().push_back(-33333);

        // Try to insert.
        expectNoThrow([&] { inserter(bar0); }).fatal("Failed to insert object");
        expectNoThrow([&] { inserter(bar1); }).fatal("Failed to insert object");

        // Verify existence of objects before and after delete.
        std::string id;
        auto        stmt = arrayTable.count().where(like(arrayTable.col<1>(), &id)).compile();
        id               = bar0.id.getAsString();
        compareEQ(2, stmt.bind(sql::BindParameters::All)());
        expectNoThrow([&] { deleter(bar0); });
        compareEQ(0, stmt.bind(sql::BindParameters::All)());
        id = bar1.id.getAsString();
        compareEQ(3, stmt.bind(sql::BindParameters::All)());
        expectNoThrow([&] { deleter(bar1); });
        compareEQ(0, stmt.bind(sql::BindParameters::All)());
    }

    // Delete Baz.
    {
        const sql::TypedTable<sql::row_id, std::string, uint32_t> array0Table(
          library->getDatabase().getTable("main_baz_uints"));
        const sql::TypedTable<sql::row_id, std::string, double> array1Table(
          library->getDatabase().getTable("main_baz_doubles"));

        auto inserter = alex::InsertQuery(BazDescriptor(bazType));
        auto deleter  = alex::DeleteQuery(BazDescriptor(bazType));

        // Create objects.
        Baz baz0;
        baz0.uints.get().push_back(10);
        baz0.uints.get().push_back(100);
        baz0.doubles.get().push_back(0.5);
        baz0.doubles.get().push_back(1.5);
        Baz baz1;
        baz1.uints.get().push_back(111);
        baz1.uints.get().push_back(2222);
        baz1.uints.get().push_back(33333);
        baz1.doubles.get().push_back(-2.5);
        baz1.doubles.get().push_back(-3.5);
        baz1.doubles.get().push_back(-4.5);
        Baz baz2;

        // Try to insert.
        expectNoThrow([&] { inserter(baz0); }).fatal("Failed to insert object");
        expectNoThrow([&] { inserter(baz1); }).fatal("Failed to insert object");
        expectNoThrow([&] { inserter(baz2); }).fatal("Failed to insert object");

        // Verify existence of objects before and after delete.
        std::string id;
        auto        stmt0 = array0Table.count().where(like(array0Table.col<1>(), &id)).compile();
        auto        stmt1 = array1Table.count().where(like(array1Table.col<1>(), &id)).compile();
        id                = baz0.id.getAsString();
        compareEQ(2, stmt0.bind(sql::BindParameters::All)());
        compareEQ(2, stmt1.bind(sql::BindParameters::All)());
        expectNoThrow([&] { deleter(baz0); });
        compareEQ(0, stmt0.bind(sql::BindParameters::All)());
        compareEQ(0, stmt1.bind(sql::BindParameters::All)());
        id = baz1.id.getAsString();
        compareEQ(3, stmt0.bind(sql::BindParameters::All)());
        compareEQ(3, stmt1.bind(sql::BindParameters::All)());
        expectNoThrow([&] { deleter(baz1); });
        compareEQ(0, stmt0.bind(sql::BindParameters::All)());
        compareEQ(0, stmt1.bind(sql::BindParameters::All)());
        id = baz2.id.getAsString();
        compareEQ(0, stmt0.bind(sql::BindParameters::All)());
        compareEQ(0, stmt1.bind(sql::BindParameters::All)());
        expectNoThrow([&] { deleter(baz2); });
        compareEQ(0, stmt0.bind(sql::BindParameters::All)());
        compareEQ(0, stmt1.bind(sql::BindParameters::All)());
    }
}
