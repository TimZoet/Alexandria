#include "alexandria_test/delete/delete_managed.h"

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

void DeleteManaged::operator()()
{
    // Create type.
    auto& fooType = library->createType("Foo");

    // Commit types.
    expectNoThrow([this]() { library->commitTypes(); }).fatal("Failed to commit types");

    // Create object handler.
    auto fooHandler = library->createObjectHandler<alex::Member<&Foo::id>>(fooType.getName());

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

    // Delete objects.
    expectNoThrow([&] {
        fooHandler->del(foo0->id);
        fooHandler->del(foo1->id);
        fooHandler->del(foo2->id);
    });

    // Check cache state.
    compareEQ(fooHandler->getCacheMethod(foo0->id), alex::CacheMethod::None);
    compareEQ(fooHandler->getCacheMethod(foo1->id), alex::CacheMethod::None);
    compareEQ(fooHandler->getCacheMethod(foo2->id), alex::CacheMethod::None);

    // Retrieval should fail.
    fooHandler->setDefaultCacheMethod(alex::CacheMethod::None);
    expectThrow([&] { static_cast<void>(fooHandler->get(foo0->id)); });
    expectThrow([&] { static_cast<void>(fooHandler->get(foo1->id)); });
    expectThrow([&] { static_cast<void>(fooHandler->get(foo2->id)); });

    // Check cache state.
    compareEQ(fooHandler->getCacheMethod(foo0->id), alex::CacheMethod::None);
    compareEQ(fooHandler->getCacheMethod(foo1->id), alex::CacheMethod::None);
    compareEQ(fooHandler->getCacheMethod(foo2->id), alex::CacheMethod::None);
}
