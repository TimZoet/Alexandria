#include "alexandria_test/get/get_primitive_array.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria/library.h"
#include "alexandria/member_types/member.h"
#include "alexandria/member_types/primitive_array.h"

namespace
{
    struct Foo
    {
        alex::InstanceId            id;
        alex::PrimitiveArray<float> floats;

        Foo() = default;

        Foo(alex::InstanceId iid, std::vector<float> ffloats) : id(iid) { floats.get() = std::move(ffloats); }
    };

    struct Bar
    {
        alex::InstanceId              id;
        alex::PrimitiveArray<int32_t> ints;

        Bar() = default;

        Bar(alex::InstanceId iid, std::vector<int32_t> iints) : id(iid) { ints.get() = std::move(iints); }
    };

    struct Baz
    {
        alex::InstanceId               id;
        alex::PrimitiveArray<uint64_t> ints;
        alex::PrimitiveArray<double>   floats;

        Baz() = default;

        Baz(alex::InstanceId iid, std::vector<uint64_t> iints, std::vector<double> ffloats) : id(iid)
        {
            ints.get()   = std::move(iints);
            floats.get() = std::move(ffloats);
        }
    };
}  // namespace

void GetPrimitiveArray::operator()()
{
    // Create type with floats.
    auto& fooType = library->createType("Foo");
    fooType.createPrimitiveArrayProperty("floats", alex::DataType::Float);

    // Create type with integers.
    auto& barType = library->createType("Bar");
    barType.createPrimitiveArrayProperty("ints", alex::DataType::Int32);

    // Create type with floats and integers.
    auto& bazType = library->createType("Baz");
    bazType.createPrimitiveArrayProperty("uints", alex::DataType::Uint64);
    bazType.createPrimitiveArrayProperty("doubles", alex::DataType::Double);

    // Commit types.
    expectNoThrow([this]() { library->commitTypes(); }).fatal("Failed to commit types");

    // Create object handlers.
    auto fooHandler =
      library->createObjectHandler<alex::Member<&Foo::id>, alex::Member<&Foo::floats>>(fooType.getName());
    auto barHandler = library->createObjectHandler<alex::Member<&Bar::id>, alex::Member<&Bar::ints>>(barType.getName());
    auto bazHandler =
      library->createObjectHandler<alex::Member<&Baz::id>, alex::Member<&Baz::ints>, alex::Member<&Baz::floats>>(
        bazType.getName());

    // Retrieve Foo.
    {
        // Create and insert objects.
        Foo foo0;
        foo0.floats.get().push_back(0.5f);
        foo0.floats.get().push_back(1.5f);
        Foo foo1;
        foo1.floats.get().push_back(-2.5f);
        foo1.floats.get().push_back(-3.5f);
        foo1.floats.get().push_back(-4.5f);
        expectNoThrow([&] { fooHandler.insert(foo0); }).fatal("Failed to insert object");
        expectNoThrow([&] { fooHandler.insert(foo1); }).fatal("Failed to insert object");

        // Try to retrieve objects.
        std::unique_ptr<Foo> foo0_get, foo1_get;
        expectNoThrow([&] { foo0_get = fooHandler.get(foo0.id); }).fatal("Failed to get object");
        expectNoThrow([&] { foo1_get = fooHandler.get(foo1.id); }).fatal("Failed to get object");

        // Compare objects.
        compareEQ(foo0.id, foo0_get->id);
        compareEQ(foo0.floats.get(), foo0_get->floats.get());
        compareEQ(foo1.id, foo1_get->id);
        compareEQ(foo1.floats.get(), foo1_get->floats.get());
    }

    // Retrieve Bar.
    {
        // Create and insert objects.
        Bar bar0;
        bar0.ints.get().push_back(10);
        bar0.ints.get().push_back(100);
        Bar bar1;
        bar1.ints.get().push_back(-111);
        bar1.ints.get().push_back(-2222);
        bar1.ints.get().push_back(-33333);
        expectNoThrow([&] { barHandler.insert(bar0); }).fatal("Failed to insert object");
        expectNoThrow([&] { barHandler.insert(bar1); }).fatal("Failed to insert object");

        // Try to retrieve objects.
        std::unique_ptr<Bar> bar0_get, bar1_get;
        expectNoThrow([&] { bar0_get = barHandler.get(bar0.id); }).fatal("Failed to get object");
        expectNoThrow([&] { bar1_get = barHandler.get(bar1.id); }).fatal("Failed to get object");

        // Compare objects.
        compareEQ(bar0.id, bar0_get->id);
        compareEQ(bar0.ints.get(), bar0_get->ints.get());
        compareEQ(bar1.id, bar1_get->id);
        compareEQ(bar1.ints.get(), bar1_get->ints.get());
    }

    // Retrieve Baz.
    {
        // Create and insert objects.
        Baz baz0;
        baz0.ints.get().push_back(10);
        baz0.ints.get().push_back(100);
        baz0.floats.get().push_back(0.5);
        baz0.floats.get().push_back(1.5);
        Baz baz1;
        baz1.ints.get().push_back(-111);
        baz1.ints.get().push_back(-2222);
        baz1.ints.get().push_back(-33333);
        baz1.floats.get().push_back(-2.5);
        baz1.floats.get().push_back(-3.5);
        baz1.floats.get().push_back(-4.5);
        expectNoThrow([&] { bazHandler.insert(baz0); }).fatal("Failed to insert object");
        expectNoThrow([&] { bazHandler.insert(baz1); }).fatal("Failed to insert object");

        // Try to retrieve objects.
        std::unique_ptr<Baz> baz0_get, baz1_get;
        expectNoThrow([&] { baz0_get = bazHandler.get(baz0.id); }).fatal("Failed to get object");
        expectNoThrow([&] { baz1_get = bazHandler.get(baz1.id); }).fatal("Failed to get object");

        // Compare objects.
        compareEQ(baz0.id, baz0_get->id);
        compareEQ(baz0.ints.get(), baz0_get->ints.get());
        compareEQ(baz0.floats.get(), baz0_get->floats.get());
        compareEQ(baz1.id, baz1_get->id);
        compareEQ(baz1.ints.get(), baz1_get->ints.get());
        compareEQ(baz1.floats.get(), baz1_get->floats.get());
    }
}
