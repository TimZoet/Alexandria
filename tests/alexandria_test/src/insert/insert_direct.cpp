#include "alexandria_test/insert/insert_direct.h"

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

void InsertDirect::operator()()
{
    // Create type.
    auto& fooType = library->createType("Foo");

    // Commit types.
    expectNoThrow([this]() { library->commitTypes(); }).fatal("Failed to commit types");

    // Get table.
    sql::TypedTable<int64_t> fooTable(library->getDatabase().getTable(fooType.getName()));

    // Create object handler.
    auto fooHandler = library->createObjectHandler<alex::Member<&Foo::id>>(fooType.getName());

    // Create objects.
    auto foo0 = std::make_shared<Foo>();
    auto foo1 = std::make_shared<Foo>();
    auto foo2 = std::make_shared<Foo>();
    foo2->id = 10;

    // Try to insert.
    expectNoThrow([&] { fooHandler->insert(*foo0); }).fatal("Failed to insert object");
    expectNoThrow([&] { fooHandler->insert(*foo1); }).fatal("Failed to insert object");
    expectThrow([&] { fooHandler->insert(*foo2); });

    // Check assigned IDs.
    compareEQ(foo0->id, alex::InstanceId(1));
    compareEQ(foo1->id, alex::InstanceId(2));

    // Select inserted object using sql and compare.
    const Foo foo0_get = fooTable.selectOne<Foo>(fooTable.col<0>() == foo0->id.get(), true)(false);
    const Foo foo1_get = fooTable.selectOne<Foo>(fooTable.col<0>() == foo1->id.get(), true)(false);

    // Compare objects.
    compareEQ(foo0->id, foo0_get.id);
    compareEQ(foo1->id, foo1_get.id);

    // Make sure instance isn't accidentally being cached.
    compareEQ(fooHandler->getCacheMethod(foo0->id), alex::CacheMethod::None);
    compareEQ(fooHandler->getCacheMethod(foo1->id), alex::CacheMethod::None);
}
