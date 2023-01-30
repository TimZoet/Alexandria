#include "alexandria_test/table_sets/table_sets_query.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria/core/type_descriptor.h"
#include "alexandria/queries/insert_query.h"
#include "alexandria/queries/table_sets.h"

namespace
{
    struct Foo
    {
        alex::InstanceId id;
        int32_t          a = 0;
    };

    struct Bar
    {
        alex::InstanceId          id;
        alex::ReferenceArray<Foo> foo;
    };

    using FooDescriptor = alex::GenerateTypeDescriptor<alex::Member<&Foo::id>, alex::Member<&Foo::a>>;

    using BarDescriptor = alex::GenerateTypeDescriptor<alex::Member<&Bar::id>, alex::Member<&Bar::foo>>;
}  // namespace

void TableSetsQuery::operator()()
{
    // Create types.
    auto& fooType = nameSpace->createType("foo");
    auto& barType = nameSpace->createType("bar");

    // Add properties to types.
    fooType.createPrimitiveProperty("int32prop", alex::DataType::Int32);
    barType.createReferenceArrayProperty("fooprop", fooType);

    // Commit types.
    expectNoThrow([&] {
        fooType.commit();
        barType.commit();
    }).fatal("Failed to commit types");

    // Insert objects.
    Bar bar0, bar1;
    expectNoThrow([&] {
        auto fooInserter = alex::InsertQuery(FooDescriptor(fooType));
        auto barInserter = alex::InsertQuery(BarDescriptor(barType));

        for (int32_t i = 0; i < 10; i++)
        {
            Foo foo;
            foo.a = i;
            fooInserter(foo);

            if (i % 3 == 0)
                bar0.foo.add(foo);
            else
                bar1.foo.add(foo);
        }

        barInserter(bar0);
        barInserter(bar1);
    });

    // Check if a valid query can be constructed from the TableSets.
    {
        auto fooTables = alex::TableSets(FooDescriptor(fooType));
        auto barTables = alex::TableSets(BarDescriptor(barType));

        int32_t sum          = 18;
        auto&   fooInstTable = fooTables.getInstanceTable();
        auto&   fooRefTable  = barTables.getReferenceArrayTable<0>();
        auto    stmt         = fooRefTable.join(sql::InnerJoin, fooInstTable)
                      .on(fooRefTable.col<2>() == fooInstTable.col<1>())
                      .selectAs<std::string>(fooRefTable.col<1>())
                      .groupBy(fooRefTable.col<1>())
                      .having(sql::sum<int32_t>(fooInstTable.col<2>()) == &sum)
                      .compileOne();

        compareEQ(bar0.id.getAsString(), stmt.bind(sql::BindParameters::All)());
        sum = 27;
        compareEQ(bar1.id.getAsString(), stmt.bind(sql::BindParameters::All)());
    }
}
