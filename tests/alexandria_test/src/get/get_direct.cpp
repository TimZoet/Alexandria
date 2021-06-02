#include "alexandria_test/get/get_direct.h"

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

void GetDirect::operator()()
{
    // Create type.
    auto& fooType = library->createType("Foo");

    // Commit types.
    expectNoThrow([this]() { library->commitTypes(); }).fatal("Failed to commit types");

    // Create object handlers.
    auto fooHandler = library->createObjectHandler<alex::Member<&Foo::id>>(fooType.getName());

    // Insert objects.
    auto foo0 = std::make_shared<Foo>();
    auto foo1 = std::make_shared<Foo>();
    auto foo2 = std::make_shared<Foo>();
    expectNoThrow([&] {
        fooHandler->setDefaultCacheMethod(alex::CacheMethod::None);
        fooHandler->insert(foo0);
    }).fatal("Failed to insert object");
    expectNoThrow([&] {
        fooHandler->setDefaultCacheMethod(alex::CacheMethod::Weak);
        fooHandler->insert(foo1);
    }).fatal("Failed to insert object");
    expectNoThrow([&] {
        fooHandler->setDefaultCacheMethod(alex::CacheMethod::Strong);
        fooHandler->insert(foo2);
    }).fatal("Failed to insert object");

    // Get objects with direct method.
    Foo foo0_get, foo1_get, foo2_get;
    expectNoThrow([&] { fooHandler->get(foo0->id, foo0_get); });
    expectNoThrow([&] { fooHandler->get(foo1->id, foo1_get); });
    expectNoThrow([&] { fooHandler->get(foo2->id, foo2_get); });
    expectThrow([&] {
        Foo foo3_get;
        fooHandler->get(alex::InstanceId(10), foo3_get);
    });

    // Compare objects.
    compareEQ(foo0->id, foo0_get.id);
    compareEQ(foo1->id, foo1_get.id);
    compareEQ(foo2->id, foo2_get.id);

    // Make sure instance isn't accidentally being cached.
    compareEQ(foo0.use_count(), static_cast<long>(1));
    compareEQ(foo1.use_count(), static_cast<long>(1));
    compareEQ(foo2.use_count(), static_cast<long>(2));
    compareEQ(fooHandler->getCacheMethod(foo0->id), alex::CacheMethod::None);
    compareEQ(fooHandler->getCacheMethod(foo1->id), alex::CacheMethod::Weak);
    compareEQ(fooHandler->getCacheMethod(foo2->id), alex::CacheMethod::Strong);
}