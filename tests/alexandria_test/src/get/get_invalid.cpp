#include "alexandria_test/get/get_invalid.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria/core/type_descriptor.h"
#include "alexandria/queries/get_query.h"
#include "alexandria/queries/insert_query.h"

namespace
{
    struct Foo
    {
        alex::InstanceId id;
        int32_t          a = 0;
    };

    using FooDescriptor = alex::GenerateTypeDescriptor<alex::Member<&Foo::id>, alex::Member<&Foo::a>>;
}  // namespace

void GetInvalid::operator()()
{
    // Create type.
    auto& fooType = nameSpace->createType("foo");
    fooType.createPrimitiveProperty("a", alex::DataType::Int32);
    expectNoThrow([&] { fooType.commit(); }).fatal("Failed to commit types");

    auto inserter = alex::InsertQuery(FooDescriptor(fooType));
    auto getter   = alex::GetQuery(FooDescriptor(fooType));
    Foo  foo;

    // Retrieving uninitialized object should throw.
    expectThrow([&] { getter(foo); });

    // Retrieving after insert should work.
    expectNoThrow([&] { inserter(foo); });
    expectNoThrow([&] { getter(foo); });
    compareTrue(foo.id.valid());

    // Retrieving non-existent object should throw.
    foo.id.regenerate();
    expectThrow([&] { getter(foo); });
}
