#include "alexandria_test/insert/insert_reference.h"

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

void InsertReference::operator()()
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

    // TODO: Once the way references to not yet inserted objects are handled is finalized, test that here as well.
    // TODO: Test insert of empty/null references.

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

    // Insert Bar.
    {
        const sql::TypedTable<sql::row_id, std::string, std::string> table(library->getDatabase().getTable("main_Bar"));

        auto inserter = alex::InsertQuery(BarDescriptor(barType));

        // Try to insert.
        expectNoThrow([&] { inserter(bar0); }).fatal("Failed to insert object");
        expectNoThrow([&] { inserter(bar1); }).fatal("Failed to insert object");

        // Check assigned IDs.
        compareTrue(bar0.id.valid());
        compareTrue(bar1.id.valid());

        // Select inserted object using sql and compare.
        std::string id;
        auto        stmt    = table.selectAs<Bar, 1, 2>().where(like(table.col<1>(), &id)).compileOne();
        id                  = bar0.id.getAsString();
        const auto bar0_get = stmt.bind(sql::BindParameters::All)();
        id                  = bar1.id.getAsString();
        const auto bar1_get = stmt.bind(sql::BindParameters::All)();

        // Compare objects.
        compareEQ(bar0.id, bar0_get.id);
        compareEQ(bar0.foo.getId(), bar0_get.foo.getId());
        compareEQ(bar1.id, bar1_get.id);
        compareEQ(bar1.foo.getId(), bar1_get.foo.getId());
    }

    // Insert Baz.
    {
        const sql::TypedTable<sql::row_id, std::string, std::string, std::string> table(
          library->getDatabase().getTable("main_Baz"));

        auto inserter = alex::InsertQuery(BazDescriptor(bazType));

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

        // Check assigned IDs.
        compareTrue(baz0.id.valid());
        compareTrue(baz1.id.valid());
        compareTrue(baz2.id.valid());
        compareTrue(baz3.id.valid());

        // Select inserted object using sql and compare.
        std::string id;
        auto        stmt    = table.selectAs<Baz, 1, 2, 3>().where(like(table.col<1>(), &id)).compileOne();
        id                  = baz0.id.getAsString();
        const auto baz0_get = stmt.bind(sql::BindParameters::All)();
        id                  = baz1.id.getAsString();
        const auto baz1_get = stmt.bind(sql::BindParameters::All)();
        id                  = baz2.id.getAsString();
        const auto baz2_get = stmt.bind(sql::BindParameters::All)();
        id                  = baz3.id.getAsString();
        const auto baz3_get = stmt.bind(sql::BindParameters::All)();

        // Compare objects.
        compareEQ(baz0.id, baz0_get.id);
        compareEQ(baz0.foo.getId(), baz0_get.foo.getId());
        compareEQ(baz0.bar.getId(), baz0_get.bar.getId());
        compareEQ(baz1.id, baz1_get.id);
        compareEQ(baz1.foo.getId(), baz1_get.foo.getId());
        compareEQ(baz1.bar.getId(), baz1_get.bar.getId());
        compareEQ(baz2.id, baz2_get.id);
        compareEQ(baz2.foo.getId(), baz2_get.foo.getId());
        compareEQ(baz2.bar.getId(), baz2_get.bar.getId());
        compareEQ(baz3.id, baz3_get.id);
        compareEQ(baz3.foo.getId(), baz3_get.foo.getId());
        compareEQ(baz3.bar.getId(), baz3_get.bar.getId());
    }
}
