#include "alexandria_test/delete/delete_reference.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria/core/library.h"
#include "alexandria/queries/delete_query.h"
#include "alexandria/queries/insert_query.h"

namespace
{
    struct Foo
    {
        alex::InstanceId id;
        float            a;
        int32_t          b;
    };

    struct Bar
    {
        alex::InstanceId     id;
        alex::Reference<Foo> foo;

        Bar() = default;
        Bar(std::string iid, std::string fooid) : id(std::move(iid)), foo(std::move(fooid)) {}
    };

    struct Baz
    {
        alex::InstanceId     id;
        alex::Reference<Foo> foo;
        alex::Reference<Bar> bar;

        Baz() = default;
        Baz(std::string iid, std::string fooid, std::string barid) :
            id(std::move(iid)), foo(std::move(fooid)), bar(std::move(barid))
        {
        }
    };

    using FooDescriptor =
      alex::GenerateTypeDescriptor<alex::Member<&Foo::id>, alex::Member<&Foo::a>, alex::Member<&Foo::b>>;

    using BarDescriptor = alex::GenerateTypeDescriptor<alex::Member<&Bar::id>, alex::Member<&Bar::foo>>;

    using BazDescriptor =
      alex::GenerateTypeDescriptor<alex::Member<&Baz::id>, alex::Member<&Baz::foo>, alex::Member<&Baz::bar>>;
}  // namespace

void DeleteReference::operator()()
{
    // Create types.
    auto& fooType = nameSpace->createType("Foo");
    auto& barType = nameSpace->createType("Bar");
    auto& bazType = nameSpace->createType("Baz");

    // Add properties to types.
    fooType.createPrimitiveProperty("floatProp", alex::DataType::Float);
    fooType.createPrimitiveProperty("int32Prop", alex::DataType::Int32);
    barType.createReferenceProperty("fooProp", fooType);
    bazType.createReferenceProperty("fooProp", fooType);
    bazType.createReferenceProperty("barProp", barType);

    // Commit types.
    expectNoThrow([&] {
        fooType.commit();
        barType.commit();
        bazType.commit();
    }).fatal("Failed to commit types");

    // Create objects.
    Foo foo0{.a = 0.5f, .b = 4};
    Foo foo1{.a = -0.5f, .b = -10};

    // Insert Foo.
    expectNoThrow([&] {
        auto inserter = alex::InsertQuery(FooDescriptor(fooType));
        inserter(foo0);
        inserter(foo1);
    }).fatal("Failed to insert object");

    Bar bar0, bar1;
    bar0.foo = foo0;
    bar1.foo = foo1;

    // Delete Bar.
    {
        const sql::TypedTable<sql::row_id, std::string, std::string> table(library->getDatabase().getTable("main_Bar"));

        auto inserter = alex::InsertQuery(BarDescriptor(barType));
        auto deleter  = alex::DeleteQuery(BarDescriptor(barType));

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

    // Delete Baz.
    {
        const sql::TypedTable<sql::row_id, std::string, std::string, std::string> table(
          library->getDatabase().getTable("main_Baz"));

        auto inserter = alex::InsertQuery(BazDescriptor(bazType));
        auto deleter  = alex::DeleteQuery(BazDescriptor(bazType));

        // Create objects.
        Baz baz0, baz1, baz2, baz3;
        baz0.foo = foo0;
        baz0.bar = bar0;
        baz1.foo = foo0;
        baz1.bar = bar1;
        baz2.foo = foo1;
        baz2.bar = bar0;
        baz3.foo = foo1;
        baz3.bar = bar1;

        // Try to insert.
        expectNoThrow([&] { inserter(baz0); }).fatal("Failed to insert object");
        expectNoThrow([&] { inserter(baz1); }).fatal("Failed to insert object");
        expectNoThrow([&] { inserter(baz2); }).fatal("Failed to insert object");
        expectNoThrow([&] { inserter(baz3); }).fatal("Failed to insert object");

        // Verify existence of objects before and after delete.
        std::string id;
        auto        stmt = table.count().where(like(table.col<1>(), &id)).compile();
        id               = baz0.id.getAsString();
        compareEQ(1, stmt.bind(sql::BindParameters::All)());
        expectNoThrow([&] { deleter(baz0); });
        compareEQ(0, stmt.bind(sql::BindParameters::All)());
        id = baz1.id.getAsString();
        compareEQ(1, stmt.bind(sql::BindParameters::All)());
        expectNoThrow([&] { deleter(baz1); });
        compareEQ(0, stmt.bind(sql::BindParameters::All)());
        id = baz2.id.getAsString();
        compareEQ(1, stmt.bind(sql::BindParameters::All)());
        expectNoThrow([&] { deleter(baz2); });
        compareEQ(0, stmt.bind(sql::BindParameters::All)());
        id = baz3.id.getAsString();
        compareEQ(1, stmt.bind(sql::BindParameters::All)());
        expectNoThrow([&] { deleter(baz3); });
        compareEQ(0, stmt.bind(sql::BindParameters::All)());
    }
}
