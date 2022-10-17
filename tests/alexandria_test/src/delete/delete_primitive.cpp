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
    fooType.createPrimitiveProperty("floatProp", alex::DataType::Float);
    fooType.createPrimitiveProperty("doubleProp", alex::DataType::Double);

    // Create type with integers.
    auto& barType = library->createType("Bar");
    barType.createPrimitiveProperty("int32Prop", alex::DataType::Int32);
    barType.createPrimitiveProperty("int64Prop", alex::DataType::Int64);
    barType.createPrimitiveProperty("uint32Prop", alex::DataType::Uint32);
    barType.createPrimitiveProperty("uint64Prop", alex::DataType::Uint64);

    // Commit types.
    expectNoThrow([this]() { library->commitTypes(); }).fatal("Failed to commit types");

    // Get tables.
    sql::TypedTable<int64_t, float, double> fooTable(library->getDatabase().getTable(fooType.getName()));
    sql::TypedTable<int64_t, int32_t, int64_t, uint32_t, uint64_t> barTable(
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
        // Create and insert objects.
        Foo foo0{.a = 0.5f, .b = 1.5};
        Foo foo1{.a = -0.5f, .b = -1.5};
        expectNoThrow([&] { fooHandler->insert(foo0); }).fatal("Failed to insert object");
        expectNoThrow([&] { fooHandler->insert(foo1); }).fatal("Failed to insert object");

        // Delete objects one by one and check tables.
        compareEQ(static_cast<size_t>(2), fooTable.countAll()(true));
        expectNoThrow([&] { fooHandler->del(foo0.id); });
        compareEQ(static_cast<size_t>(1), fooTable.countAll()(true));
        expectNoThrow([&] { fooHandler->del(foo1.id); });
        compareEQ(static_cast<size_t>(0), fooTable.countAll()(true));
    }

    // Delete Bar.
    {
        // Create and insert objects.
        Bar bar0{.a = 1, .b = 2, .c = 3, .d = 4};
        Bar bar1{.a = -1, .b = -2, .c = 123456, .d = 1234567};
        expectNoThrow([&] { barHandler->insert(bar0); }).fatal("Failed to insert object");
        expectNoThrow([&] { barHandler->insert(bar1); }).fatal("Failed to insert object");

        // Delete objects one by one and check tables.
        compareEQ(static_cast<size_t>(2), barTable.countAll()(true));
        expectNoThrow([&] { barHandler->del(bar0.id); });
        compareEQ(static_cast<size_t>(1), barTable.countAll()(true));
        expectNoThrow([&] { barHandler->del(bar1.id); });
        compareEQ(static_cast<size_t>(0), barTable.countAll()(true));
    }
}
