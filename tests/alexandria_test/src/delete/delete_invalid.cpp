#include "alexandria_test/delete/delete_invalid.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria-core/type_descriptor.h"
#include "alexandria-query/delete_query.h"
#include "alexandria-query/insert_query.h"

namespace
{
    struct Foo
    {
        alex::InstanceId id;
        int32_t          a = 0;
    };

    using FooDescriptor = alex::GenerateTypeDescriptor<alex::Member<"id", &Foo::id>, alex::Member<"a", &Foo::a>>;
}  // namespace

void DeleteInvalid::operator()()
{
    // Create type.
    auto& fooType = nameSpace->createType("foo");
    fooType.createPrimitiveProperty("a", alex::DataType::Int32);
    expectNoThrow([&] { fooType.commit(); }).fatal("Failed to commit types");

    auto inserter = alex::InsertQuery(FooDescriptor(fooType));
    auto deleter  = alex::DeleteQuery(FooDescriptor(fooType));
    bool deleted  = false;
    Foo  foo;

    // Deleting uninitialized object should throw.
    expectThrow([&] { deleter(foo); });
    compareFalse(foo.id.valid());

    // Deleting after insert should work.
    expectNoThrow([&] { inserter(foo); });
    expectNoThrow([&] { deleted = deleter(foo); });
    compareTrue(deleted);
    compareFalse(foo.id.valid());

    // Deleting non-existent object should fail.
    foo.id.regenerate();
    expectNoThrow([&] { deleted = deleter(foo); });
    compareFalse(deleted);
    compareTrue(foo.id.valid());
}
