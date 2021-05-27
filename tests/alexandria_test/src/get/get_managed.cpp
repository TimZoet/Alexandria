#include "alexandria_test/get/get_managed.h"

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

void GetManaged::operator()()
{
    // Create type.
    auto& fooType = library->createType("Foo");

    // Commit types.
    expectNoThrow([this]() { library->commitTypes(); }).fatal("Failed to commit types");

    // Create object handler.
    auto fooHandler = library->createObjectHandler<alex::Member<&Foo::id>>(fooType);

    std::shared_ptr<Foo> foo0, foo1, foo2;

    // Create objects with different cache method.
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

    // Check reference counts and cache state.
    compareEQ(foo0.use_count(), static_cast<long>(1));
    compareEQ(foo1.use_count(), static_cast<long>(1));
    compareEQ(foo2.use_count(), static_cast<long>(2));
    compareEQ(fooHandler->getCache(foo0), alex::CacheMethod::None);
    compareEQ(fooHandler->getCache(foo1), alex::CacheMethod::Weak);
    compareEQ(fooHandler->getCache(foo2), alex::CacheMethod::Strong);

    // Reset pointers and then retrieve.
    foo0.reset();
    foo1.reset();
    foo2.reset();
    fooHandler->setDefaultCacheMethod(alex::CacheMethod::None);
    expectNoThrow([&] { foo0 = fooHandler->get(1); }).fatal("Failed to get object");
    expectNoThrow([&] { foo1 = fooHandler->get(2); }).fatal("Failed to get object");
    expectNoThrow([&] { foo2 = fooHandler->get(3); }).fatal("Failed to get object");
    expectThrow([&] { static_cast<void>(fooHandler->get(4)); });

    // Check cache state.
    compareEQ(fooHandler->getCache(foo0), alex::CacheMethod::None);
    compareEQ(fooHandler->getCache(foo1), alex::CacheMethod::None);
    compareEQ(fooHandler->getCache(foo2), alex::CacheMethod::Strong);
}
