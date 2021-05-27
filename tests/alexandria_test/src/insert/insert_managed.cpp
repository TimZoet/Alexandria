#include "alexandria_test/insert/insert_managed.h"

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

void InsertManaged::operator()()
{
    // Create type.
    auto& fooType = library->createType("Foo");

    // Commit types.
    expectNoThrow([this]() { library->commitTypes(); }).fatal("Failed to commit types");

    // Create object handler.
    auto fooHandler = library->createObjectHandler<alex::Member<&Foo::id>>(fooType);

    auto foo0 = std::make_shared<Foo>();
    auto foo1 = std::make_shared<Foo>();
    auto foo2 = std::make_shared<Foo>();

    // Insert objects with different cache method.
    expectNoThrow([&] {
        fooHandler->setDefaultCacheMethod(alex::CacheMethod::None);
        fooHandler->insert(foo0);
    }).fatal("Failed to create object");
    expectNoThrow([&] {
        fooHandler->setDefaultCacheMethod(alex::CacheMethod::Weak);
        fooHandler->insert(foo1);
    }).fatal("Failed to create object");
    expectNoThrow([&] {
        fooHandler->setDefaultCacheMethod(alex::CacheMethod::Strong);
        fooHandler->insert(foo2);
    }).fatal("Failed to create object");
    expectThrow([&] {
        auto foo3 = std::make_shared<Foo>();
        foo3->id  = 10;
        fooHandler->insert(foo3);
    });
    expectThrow([&] { fooHandler->insert(nullptr); });

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

    // Check cache state.
    compareEQ(fooHandler->getCache(foo0), alex::CacheMethod::None);
    compareEQ(fooHandler->getCache(foo1), alex::CacheMethod::None);
    compareEQ(fooHandler->getCache(foo2), alex::CacheMethod::Strong);
}
