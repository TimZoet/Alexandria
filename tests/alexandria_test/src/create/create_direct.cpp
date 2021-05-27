#include "alexandria_test/create/create_direct.h"

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
    };
}  // namespace

void CreateDirect::operator()()
{
    // Create type.
    auto& fooType = library->createType("Foo");

    // Commit types.
    expectNoThrow([this]() { library->commitTypes(); }).fatal("Failed to commit types");

    // Get table.
    sql::ext::TypedTable<int64_t> fooTable(library->getDatabase().getTable(fooType.getName()));

    // Create object handler.
    auto fooHandler = library->createObjectHandler<alex::Member<&Foo::id>>(fooType);
    fooHandler->setDefaultCacheMethod(alex::CacheMethod::None);

    // Create objects with direct methods.
    auto foo0 = std::make_shared<Foo>();
    auto foo1 = std::make_shared<Foo>();
    expectNoThrow([&] { fooHandler->create(foo0.get()); }).fatal("Failed to create object");
    expectNoThrow([&] { fooHandler->create(foo1.get()); }).fatal("Failed to create object");
    expectNoThrow([&] { fooHandler->create(nullptr); }).fatal("Failed to create object");
    expectThrow([&] {
        Foo foo2;
        foo2.id = 10;
        fooHandler->create(&foo2);
    });

    // Select created object using sql and compare.
    const Foo foo0_get = fooTable.selectOne<Foo>(fooTable.col<0>() == foo0->id.get(), true)(false);
    const Foo foo1_get = fooTable.selectOne<Foo>(fooTable.col<0>() == foo1->id.get(), true)(false);

    // Compare objects.
    compareEQ(foo0->id, foo0_get.id);
    compareEQ(foo1->id, foo1_get.id);

    // Make sure instance isn't accidentally being cached.
    compareEQ(foo0.use_count(), static_cast<long>(1));
    compareEQ(foo1.use_count(), static_cast<long>(1));
    compareEQ(fooHandler->getCache(foo0), alex::CacheMethod::None);
    compareEQ(fooHandler->getCache(foo1), alex::CacheMethod::None);
}
