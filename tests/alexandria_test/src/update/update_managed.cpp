#include "alexandria_test/update/update_managed.h"

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

void UpdateManaged::operator()()
{
    // Create type.
    auto& fooType = library->createType("Foo");

    // Commit types.
    expectNoThrow([this]() { library->commitTypes(); }).fatal("Failed to commit types");

    // Create object handler.
    auto fooHandler = library->createObjectHandler<alex::Member<&Foo::id>>(fooType.getName());

    // Create objects with direct method so that they are not cached.
    auto foo0 = std::make_shared<Foo>();
    auto foo1 = std::make_shared<Foo>();
    auto foo2 = std::make_shared<Foo>();
    expectNoThrow([&] {
        fooHandler->create(foo0.get());
        fooHandler->create(foo1.get());
        fooHandler->create(foo2.get());
    }).fatal("Failed to create object");

    // Update objects with managed method and different cache methods.
    expectNoThrow([&] {
        fooHandler->setDefaultCacheMethod(alex::CacheMethod::None);
        fooHandler->update(foo0);
    });
    expectNoThrow([&] {
        fooHandler->setDefaultCacheMethod(alex::CacheMethod::Weak);
        fooHandler->update(foo1);
    });
    expectNoThrow([&] {
        fooHandler->setDefaultCacheMethod(alex::CacheMethod::Strong);
        fooHandler->update(foo2);
    });

    // Make sure objects are being appropriately cached.
    compareEQ(fooHandler->getCacheMethod(foo0->id), alex::CacheMethod::None);
    compareEQ(fooHandler->getCacheMethod(foo1->id), alex::CacheMethod::Weak);
    compareEQ(fooHandler->getCacheMethod(foo2->id), alex::CacheMethod::Strong);

    // Update with nullptr.
    expectThrow([&] { fooHandler->update(nullptr); });

    // Update non-existing object.
    expectThrow([&] {
        auto foo3 = std::make_shared<Foo>();
        foo3->id  = alex::InstanceId(10);
        fooHandler->update(foo3);
    });
}
