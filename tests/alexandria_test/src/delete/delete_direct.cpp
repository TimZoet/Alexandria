#include "alexandria_test/delete/delete_direct.h"

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

void DeleteDirect::operator()()
{
    // Create type.
    auto& fooType = library->createType("Foo");

    // Commit types.
    expectNoThrow([this]() { library->commitTypes(); }).fatal("Failed to commit types");

    // Create object handler.
    auto fooHandler = library->createObjectHandler<alex::Member<&Foo::id>>(fooType);
    fooHandler->setDefaultCacheMethod(alex::CacheMethod::None);

    // Create objects with direct methods.
    auto foo = std::make_shared<Foo>();
    expectNoThrow([&] { fooHandler->create(foo.get()); }).fatal("Failed to create object");
    expectNoThrow([&] { fooHandler->create(nullptr); }).fatal("Failed to create object");

    // Delete objects.
    expectNoThrow([&] {
        fooHandler->del(foo->id);
        fooHandler->del(alex::InstanceId(2));
    });

    // Retrieval should fail.
    fooHandler->setDefaultCacheMethod(alex::CacheMethod::None);
    expectThrow([&] { static_cast<void>(fooHandler->get(foo->id)); });
    expectThrow([&] { static_cast<void>(fooHandler->get(alex::InstanceId(2))); });
}
