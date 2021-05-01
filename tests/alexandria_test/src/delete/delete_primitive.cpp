#include "alexandria_test/delete/delete_primitive.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria/library.h"
#include "alexandria/member_types/member.h"

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
}  // namespace

void DeletePrimitive::operator()()
{
    // Create type with floats.
    auto& fooType = library->createType("Foo");
    fooType.createPrimitiveProperty("floatProp", alex::DataType::Float).setDefaultValue(5.0f);
    fooType.createPrimitiveProperty("doubleProp", alex::DataType::Double).setDefaultValue(10.0);

    // Create type with integers.
    auto& barType = library->createType("Bar");
    barType.createPrimitiveProperty("int32Prop", alex::DataType::Int32).setDefaultValue(static_cast<int32_t>(-10));
    barType.createPrimitiveProperty("int64Prop", alex::DataType::Int64).setDefaultValue(static_cast<int64_t>(-20));
    barType.createPrimitiveProperty("uint32Prop", alex::DataType::Uint32).setDefaultValue(static_cast<uint32_t>(30));
    barType.createPrimitiveProperty("uint64Prop", alex::DataType::Uint64).setDefaultValue(static_cast<uint64_t>(40));

    // Commit types.
    expectNoThrow([this]() { library->commitTypes(); }).fatal("Failed to commit types");

    // Get tables.
    sql::ext::TypedTable<int64_t, float, double> fooTable(library->getDatabase().getTable(fooType.getName()));
    sql::ext::TypedTable<int64_t, int32_t, int64_t, uint32_t, uint64_t> barTable(
      library->getDatabase().getTable(barType.getName()));

    // Create object handlers.
    auto fooHandler =
      library->createObjectHandler<alex::Member<&Foo::id>, alex::Member<&Foo::a>, alex::Member<&Foo::b>>(
        fooType.getName());
    auto barHandler = library->createObjectHandler<alex::Member<&Bar::id>,
                                                   alex::Member<&Bar::a>,
                                                   alex::Member<&Bar::b>,
                                                   alex::Member<&Bar::c>,
                                                   alex::Member<&Bar::d>>(barType.getName());

    // Delete Foo.
    {
        // Create objects.
        std::unique_ptr<Foo> foo0, foo1;
        expectNoThrow([&] { foo0 = fooHandler.create(); }).fatal("Failed to create object");
        expectNoThrow([&] { foo1 = fooHandler.create(); }).fatal("Failed to create object");

        // Delete objects one by one and check if they this exist in database.
        // Test using the exists method and by directly querying the database.
        compareTrue(fooHandler.exists(foo0->id));
        compareTrue(fooHandler.exists(foo1->id));
        expectNoThrow([&] { static_cast<void>(fooTable.selectOne(fooTable.col<0>() == foo0->id.get(), true)(false)); });
        expectNoThrow([&] { static_cast<void>(fooTable.selectOne(fooTable.col<0>() == foo1->id.get(), true)(false)); });

        expectNoThrow([&] { fooHandler.del(*foo0); });
        compareFalse(fooHandler.exists(foo0->id));
        compareTrue(fooHandler.exists(foo1->id));
        expectThrow([&] { static_cast<void>(fooTable.selectOne(fooTable.col<0>() == foo0->id.get(), true)(false)); });
        expectNoThrow([&] { static_cast<void>(fooTable.selectOne(fooTable.col<0>() == foo1->id.get(), true)(false)); });

        expectNoThrow([&] { fooHandler.del(*foo1); });
        compareFalse(fooHandler.exists(foo0->id));
        compareFalse(fooHandler.exists(foo1->id));
        expectThrow([&] { static_cast<void>(fooTable.selectOne(fooTable.col<0>() == foo0->id.get(), true)(false)); });
        expectThrow([&] { static_cast<void>(fooTable.selectOne(fooTable.col<0>() == foo1->id.get(), true)(false)); });
    }

    // Delete Bar.
    {
        // Create objects.
        std::unique_ptr<Bar> bar0, bar1;
        expectNoThrow([&] { bar0 = barHandler.create(); }).fatal("Failed to create object");
        expectNoThrow([&] { bar1 = barHandler.create(); }).fatal("Failed to create object");

        // Delete objects one by one and check if they this exist in database.
        // Test using the exists method and by directly querying the database.
        compareTrue(barHandler.exists(bar0->id));
        compareTrue(barHandler.exists(bar1->id));
        expectNoThrow([&] { static_cast<void>(barTable.selectOne(barTable.col<0>() == bar0->id.get(), true)(false)); });
        expectNoThrow([&] { static_cast<void>(barTable.selectOne(barTable.col<0>() == bar1->id.get(), true)(false)); });

        expectNoThrow([&] { barHandler.del(*bar0); });
        compareFalse(barHandler.exists(bar0->id));
        compareTrue(barHandler.exists(bar1->id));
        expectThrow([&] { static_cast<void>(barTable.selectOne(barTable.col<0>() == bar0->id.get(), true)(false)); });
        expectNoThrow([&] { static_cast<void>(barTable.selectOne(barTable.col<0>() == bar1->id.get(), true)(false)); });

        expectNoThrow([&] { barHandler.del(*bar1); });
        compareFalse(barHandler.exists(bar0->id));
        compareFalse(barHandler.exists(bar1->id));
        expectThrow([&] { static_cast<void>(barTable.selectOne(barTable.col<0>() == bar0->id.get(), true)(false)); });
        expectThrow([&] { static_cast<void>(barTable.selectOne(barTable.col<0>() == bar1->id.get(), true)(false)); });
    }
}
