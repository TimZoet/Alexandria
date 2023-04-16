#include "alexandria-basic-query_test/get/get_invalid.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria-core/type_descriptor.h"
#include "alexandria-basic-query/get_query.h"
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

void GetInvalid::operator()()
{
    expectNoThrow([&] {
        alex::TypeLayout fooLayout;
        fooLayout.createPrimitiveProperty("prop0", alex::DataType::Int32);
        fooLayout.commit(*nameSpace, "foo");
    }).fatal("Failed to commit types");

    auto& fooType = nameSpace->getType("foo");

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
