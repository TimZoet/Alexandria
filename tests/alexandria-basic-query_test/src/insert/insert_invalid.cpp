#include "alexandria-basic-query_test/insert/insert_invalid.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria-core/type_descriptor.h"
#include "alexandria-basic-query/insert_query.h"

namespace
{
    struct Foo
    {
        alex::InstanceId id;
        int32_t          a = 0;
    };

    using FooDescriptor = alex::GenerateTypeDescriptor<alex::Member<"id", &Foo::id>, alex::Member<"a", &Foo::a>>;
}  // namespace

void InsertInvalid::operator()()
{
    // Create type.
    auto& fooType = nameSpace->createType("foo");
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
