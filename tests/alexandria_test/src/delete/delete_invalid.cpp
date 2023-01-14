#include "alexandria_test/delete/delete_invalid.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria/core/type_descriptor.h"
#include "alexandria/queries/delete_query.h"
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

void DeleteInvalid::operator()()
{
    // Create type.
    auto& fooType = nameSpace->createType("Foo");
    fooType.createPrimitiveProperty("a", alex::DataType::Int32);
    expectNoThrow([&] { fooType.commit(); }).fatal("Failed to commit types");

    auto inserter = alex::InsertQuery(FooDescriptor(fooType));
    auto deleter  = alex::DeleteQuery(FooDescriptor(fooType));
    Foo  foo;

    // Deleting uninitialized object should throw.
    expectThrow([&] { deleter(foo); });
    compareFalse(foo.id.valid());

    // Deleting after insert should work.
    expectNoThrow([&] { inserter(foo); });
    expectNoThrow([&] { deleter(foo); });
    compareTrue(foo.id.valid());

    // Deleting non-existent object should 'fail' silently.
    foo.id.regenerate();
    expectNoThrow([&] { deleter(foo); });
}
