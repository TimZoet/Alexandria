#include "alexandria-extended-query_test/search_queries/primitive_search.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria-core/type_descriptor.h"
#include "alexandria-basic-query/insert_query.h"
#include "alexandria-extended-query/search_queries/primitive_search.h"

namespace
{
    struct Foo
    {
        alex::InstanceId id;
        float            a = 0.0f;
        int32_t          b = 0;
        std::string      c;
    };

    using FooDescriptor = alex::GenerateTypeDescriptor<alex::Member<"id", &Foo::id>,
                                                       alex::Member<"a", &Foo::a>,
                                                       alex::Member<"b", &Foo::b>,
                                                       alex::Member<"c", &Foo::c>>;
}  // namespace

void PrimitiveSearch::operator()()
{
    // Create type.
    auto& fooType = nameSpace->createType("foo");
    fooType.createPrimitiveProperty("a", alex::DataType::Float);
    fooType.createPrimitiveProperty("b", alex::DataType::Int32);
    fooType.createStringProperty("c");

    // Commit types.
    expectNoThrow([&] { fooType.commit(); }).fatal("Failed to commit types");

    Foo foo0{.a = 10, .b = -33, .c = "abc"};
    Foo foo1{.a = 20, .b = -33, .c = "def"};
    Foo foo2{.a = 30, .b = -66, .c = "ghi"};
    Foo foo3{.a = 40, .b = -66, .c = "jkl"};

    auto fooDescriptor = FooDescriptor(fooType);
    auto inserter      = alex::InsertQuery(fooDescriptor);
    inserter(foo0);
    inserter(foo1);
    inserter(foo2);
    inserter(foo3);

    /*
     * Test search with single primitive property.
     */

    {
        auto query = alex::primitiveSearch(fooDescriptor, alex::equal<FooDescriptor, "a">());
        query(10);
        std::vector<alex::InstanceId> ids(query.begin(), query.end());
        compareEQ(std::vector{foo0.id}, ids);

        query(20);
        ids.assign(query.begin(), query.end());
        compareEQ(std::vector{foo1.id}, ids);
    }

    {
        auto query = alex::primitiveSearch(fooDescriptor, alex::notEqual<FooDescriptor, "a">());
        query(10);
        std::vector<alex::InstanceId> ids(query.begin(), query.end());
        compareEQ(std::vector{foo1.id, foo2.id, foo3.id}, ids);

        query(20);
        ids.assign(query.begin(), query.end());
        compareEQ(std::vector{foo0.id, foo2.id, foo3.id}, ids);
    }

    {
        auto query = alex::primitiveSearch(fooDescriptor, alex::less<FooDescriptor, "a">());
        query(30);
        std::vector<alex::InstanceId> ids(query.begin(), query.end());
        compareEQ(std::vector{foo0.id, foo1.id}, ids);

        query(20);
        ids.assign(query.begin(), query.end());
        compareEQ(std::vector{foo0.id}, ids);
    }

    {
        auto query = alex::primitiveSearch(fooDescriptor, alex::greater<FooDescriptor, "a">());
        query(30);
        std::vector<alex::InstanceId> ids(query.begin(), query.end());
        compareEQ(std::vector{foo3.id}, ids);

        query(20);
        ids.assign(query.begin(), query.end());
        compareEQ(std::vector{foo2.id, foo3.id}, ids);
    }

    {
        auto query = alex::primitiveSearch(fooDescriptor, alex::lessEqual<FooDescriptor, "a">());
        query(30);
        std::vector<alex::InstanceId> ids(query.begin(), query.end());
        compareEQ(std::vector{foo0.id, foo1.id, foo2.id}, ids);

        query(10);
        ids.assign(query.begin(), query.end());
        compareEQ(std::vector{foo0.id}, ids);
    }

    {
        auto query = alex::primitiveSearch(fooDescriptor, alex::greaterEqual<FooDescriptor, "a">());
        query(10);
        std::vector<alex::InstanceId> ids(query.begin(), query.end());
        compareEQ(std::vector{foo0.id, foo1.id, foo2.id, foo3.id}, ids);

        query(40);
        ids.assign(query.begin(), query.end());
        compareEQ(std::vector{foo3.id}, ids);
    }

    /*
     * Test search with multiple primitive properties.
     */

    {
        auto query =
          alex::primitiveSearchAnd(fooDescriptor, alex::equal<FooDescriptor, "a">(), alex::equal<FooDescriptor, "b">());
        query(10, -33);
        std::vector<alex::InstanceId> ids(query.begin(), query.end());
        compareEQ(std::vector{foo0.id}, ids);

        query(40, -66);
        ids.assign(query.begin(), query.end());
        compareEQ(std::vector{foo3.id}, ids);
    }

    {
        auto query = alex::primitiveSearchOr(
          fooDescriptor, alex::equal<FooDescriptor, "c">(), alex::greater<FooDescriptor, "a">());
        query("abc", 30);
        std::vector<alex::InstanceId> ids(query.begin(), query.end());
        compareEQ(std::vector{foo0.id, foo3.id}, ids);

        query("jkl", 90);
        ids.assign(query.begin(), query.end());
        compareEQ(std::vector{foo3.id}, ids);
    }
}
