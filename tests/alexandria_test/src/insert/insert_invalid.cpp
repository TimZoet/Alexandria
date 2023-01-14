#include "alexandria_test/insert/insert_invalid.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria/core/type_descriptor.h"
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

void InsertInvalid::operator()()
{
    // Create type.
    auto& fooType = nameSpace->createType("Foo");
    fooType.createPrimitiveProperty("a", alex::DataType::Int32);
    expectNoThrow([&] { fooType.commit(); }).fatal("Failed to commit types");

    auto inserter = alex::InsertQuery(FooDescriptor(fooType));
    Foo  foo;

    // Inserting uninitialized object should work.
    expectNoThrow([&] { inserter(foo); });
    compareTrue(foo.id.valid());

    // Inserting initialized object should throw.
    expectThrow([&] { inserter(foo); });
}
