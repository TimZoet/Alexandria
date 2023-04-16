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
    expectNoThrow([&] {
        alex::TypeLayout fooLayout;
        fooLayout.createPrimitiveProperty("prop0", alex::DataType::Int32);
        fooLayout.commit(*nameSpace, "foo");
    }).fatal("Failed to commit types");

    auto& fooType = nameSpace->getType("foo");

    auto inserter = alex::InsertQuery(FooDescriptor(fooType));
    Foo  foo;

    // Inserting uninitialized object should work.
    expectNoThrow([&] { inserter(foo); });
    compareTrue(foo.id.valid());

    // Inserting initialized object should throw.
    expectThrow([&] { inserter(foo); });
}
