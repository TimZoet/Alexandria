#include "alexandria_test/update/update_direct.h"

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

void UpdateDirect::operator()()
{
    // Create type.
    auto& fooType = library->createType("Foo");

    // Commit types.
    expectNoThrow([this]() { library->commitTypes(); }).fatal("Failed to commit types");

    // Create object handler.
    auto fooHandler = library->createObjectHandler<alex::Member<&Foo::id>>(fooType.getName());

    // Create objects.
    std::shared_ptr<Foo> foo0, foo1, foo2;
    expectNoThrow([&] {
        fooHandler->setDefaultCacheMethod(alex::CacheMethod::None);
        foo0 = fooHandler->create();
    }).fatal("Failed to create object");
    expectNoThrow([&] {
        fooHandler->setDefaultCacheMethod(alex::CacheMethod::Weak);
        foo1 = fooHandler->create();
    }).fatal("Failed to create object");
    expectNoThrow([&] {
        fooHandler->setDefaultCacheMethod(alex::CacheMethod::Strong);
        foo2 = fooHandler->create();
    }).fatal("Failed to create object");

    // Update objects with direct method.
    expectNoThrow([&] { fooHandler->update(*foo0); });
    expectNoThrow([&] { fooHandler->update(*foo1); });
    expectNoThrow([&] { fooHandler->update(*foo2); });

    // Update non-existing object.
    expectThrow([&] {
        Foo foo3;
        foo3.id = 10;
        fooHandler->update(foo3);
    });

    // Make sure instance isn't accidentally being cached.
    compareEQ(fooHandler->getCache(foo0), alex::CacheMethod::None);
    compareEQ(fooHandler->getCache(foo1), alex::CacheMethod::Weak);
    compareEQ(fooHandler->getCache(foo2), alex::CacheMethod::Strong);
}
