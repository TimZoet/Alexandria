#include "alexandria-basic-query_test/update/update_invalid.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria-core/type_descriptor.h"
#include "alexandria-basic-query/get_query.h"
#include "alexandria-basic-query/insert_query.h"
#include "alexandria-basic-query/update_query.h"

namespace
{
    struct Foo
    {
        alex::InstanceId id;
        int32_t          a = 0;
    };

    using FooDescriptor = alex::GenerateTypeDescriptor<alex::Member<"id", &Foo::id>, alex::Member<"a", &Foo::a>>;
}  // namespace

void UpdateInvalid::operator()()
{
    expectNoThrow([&] {
        alex::TypeLayout fooLayout;
        fooLayout.createPrimitiveProperty("prop0", alex::DataType::Int32);
        fooLayout.commit(*nameSpace, "foo");
    }).fatal("Failed to commit types");

    auto& fooType = nameSpace->getType("foo");

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
