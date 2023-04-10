#include "alexandria-extended-query_test/search_queries/reference_search.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria-core/type_descriptor.h"
#include "alexandria-basic-query/insert_query.h"
#include "alexandria-extended-query/search_queries/primitive_search.h"
#include "alexandria-extended-query/search_queries/reference_search.h"

namespace
{
    struct Foo
    {
        alex::InstanceId id;
        int32_t          a = 0;
    };

    struct Bar
    {
        alex::InstanceId     id;
        alex::Reference<Foo> foo;
        alex::Reference<Foo> foo2;
    };

    struct Baz
    {
        alex::InstanceId          id;
        alex::ReferenceArray<Foo> foos;
        alex::ReferenceArray<Bar> bars;
    };

    using FooDescriptor = alex::GenerateTypeDescriptor<alex::Member<"id", &Foo::id>, alex::Member<"a", &Foo::a>>;
    using BarDescriptor = alex::GenerateTypeDescriptor<alex::Member<"id", &Bar::id>,
                                                       alex::Member<"foo", &Bar::foo>,
                                                       alex::Member<"foo2", &Bar::foo2>>;
    using BazDescriptor = alex::GenerateTypeDescriptor<alex::Member<"id", &Baz::id>,
                                                       alex::Member<"foos", &Baz::foos>,
                                                       alex::Member<"bars", &Baz::bars>>;
}  // namespace

void ReferenceSearch::operator()()
{
    // Create types.
    auto& fooType = nameSpace->createType("foo");
    fooType.createPrimitiveProperty("a", alex::DataType::Int32);
    auto& barType = nameSpace->createType("bar");
    barType.createReferenceProperty("foo", fooType);
    barType.createReferenceProperty("foo2", fooType);
    auto& bazType = nameSpace->createType("baz");
    bazType.createReferenceArrayProperty("foos", fooType);
    bazType.createReferenceArrayProperty("bars", barType);

    // Commit types.
    expectNoThrow([&] {
        fooType.commit();
        barType.commit();
        bazType.commit();
    }).fatal("Failed to commit types");

    auto fooDescriptor = FooDescriptor(fooType);
    auto barDescriptor = BarDescriptor(barType);
    auto bazDescriptor = BazDescriptor(bazType);

    Foo foo0{.a = 10};
    Foo foo1{.a = 20};
    Foo foo2{.a = 30};
    Foo foo3{.a = 40};
    expectNoThrow([&] {
        auto inserter = alex::InsertQuery(fooDescriptor);
        inserter(foo0);
        inserter(foo1);
        inserter(foo2);
        inserter(foo3);
    });

    Bar bar0, bar1, bar2, bar3, bar4;
    bar0.foo  = foo0;
    bar0.foo2 = foo1;
    bar1.foo  = foo1;
    bar2.foo  = foo2;
    bar2.foo2 = foo3;
    bar3.foo  = foo2;
    bar3.foo2 = foo0;
    expectNoThrow([&] {
        auto inserter = alex::InsertQuery(barDescriptor);
        inserter(bar0);
        inserter(bar1);
        inserter(bar2);
        inserter(bar3);
        inserter(bar4);
    });

    Baz baz0, baz1;
    baz0.foos.add(foo0);
    baz0.foos.add(foo1);
    baz0.bars.add(bar0);
    baz1.foos.add(foo1);
    baz1.foos.add(foo2);
    baz1.bars.add(bar2);
    baz1.bars.add(bar3);
    expectNoThrow([&] {
        auto inserter = alex::InsertQuery(bazDescriptor);
        inserter(baz0);
        inserter(baz1);
    });

    /*
     * Test search with reference property.
     */

    {
        auto query = alex::primitiveSearch(barDescriptor, alex::equal<BarDescriptor, "foo">());
        query(foo0.id);
        std::vector<alex::InstanceId> ids(query.begin(), query.end());
        compareEQ(std::vector{bar0.id}, ids);

        query(foo2.id);
        ids.assign(query.begin(), query.end());
        compareEQ(std::vector{bar2.id, bar3.id}, ids);

        query(foo3.id);
        ids.assign(query.begin(), query.end());
        compareTrue(ids.empty());
    }

    {
        auto query = alex::primitiveSearchOr(
          barDescriptor, alex::equal<BarDescriptor, "foo">(), alex::equal<BarDescriptor, "foo2">());
        query(foo0.id, foo0.id);
        std::vector<alex::InstanceId> ids(query.begin(), query.end());
        compareEQ(std::vector{bar0.id, bar3.id}, ids);

        query(foo2.id, foo1.id);
        ids.assign(query.begin(), query.end());
        compareEQ(std::vector{bar0.id, bar2.id, bar3.id}, ids);

        query(foo3.id, foo2.id);
        ids.assign(query.begin(), query.end());
        compareTrue(ids.empty());
    }

    {
        auto query = alex::primitiveSearchOr(
          barDescriptor, alex::none<BarDescriptor, "foo">(), alex::none<BarDescriptor, "foo2">());
        query(nullptr, nullptr);
        std::vector<alex::InstanceId> ids(query.begin(), query.end());
        compareEQ(std::vector{bar1.id, bar4.id}, ids);
    }

    /*
     * Test search with reference array property.
     */

    {
        auto query = alex::referenceSearchOr(
          bazDescriptor, alex::references<BazDescriptor, "foos">(), alex::references<BazDescriptor, "bars">());

        query(foo0.id, bar3.id);
        std::vector<alex::InstanceId> ids(query.begin(), query.end());
        compareEQ(2, ids.size()).fatal("Expected 2 results.");
        // Order of results is not guaranteed.
        if (ids[0] == baz0.id)
            compareEQ(std::vector{baz0.id, baz1.id}, ids);
        else
            compareEQ(std::vector{baz1.id, baz0.id}, ids);
    }

    {
        auto query = alex::referenceSearchAnd(
          bazDescriptor, alex::references<BazDescriptor, "foos">(), alex::references<BazDescriptor, "bars">());

        query(foo1.id, bar2.id);
        std::vector<alex::InstanceId> ids(query.begin(), query.end());
        compareEQ(std::vector{baz1.id}, ids);
    }
}
