#include "alexandria_test/update/update_invalid.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria/core/type_descriptor.h"
#include "alexandria/queries/get_query.h"
#include "alexandria/queries/insert_query.h"
#include "alexandria/queries/update_query.h"

namespace
{
    struct Foo
    {
        alex::InstanceId id;
        int32_t          a = 0;
    };

    using FooDescriptor = alex::GenerateTypeDescriptor<alex::Member<&Foo::id>, alex::Member<&Foo::a>>;
}  // namespace

void UpdateInvalid::operator()()
{
    // Create type.
    auto& fooType = nameSpace->createType("foo");
    fooType.createPrimitiveProperty("a", alex::DataType::Int32);
    expectNoThrow([&] { fooType.commit(); }).fatal("Failed to commit types");

    auto inserter = alex::InsertQuery(FooDescriptor(fooType));
    auto updater  = alex::UpdateQuery(FooDescriptor(fooType));
    bool updated  = false;
    Foo  foo;

    // Updating uninitialized object should throw.
    expectThrow([&] { updater(foo); });

    // Updating initialized object should work.
    expectNoThrow([&] { inserter(foo); });
    expectNoThrow([&] { updated = updater(foo); });
    compareTrue(updated);

    // Updating non-existent object should fail.
    foo.id.regenerate();
    expectNoThrow([&] { updated = updater(foo); });
    compareFalse(updated);
}
