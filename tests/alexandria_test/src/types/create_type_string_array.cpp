#include "alexandria_test/types/create_type_string_array.h"

void CreateTypeStringArray::operator()()
{
    // Create type and property.
    alex::Type*     type;
    alex::Property *prop0, *prop1;
    expectNoThrow([&type, &prop0, &prop1, this]() {
        type  = &library->createType("type0");
        prop0 = &type->createStringArrayProperty("prop0");
        prop1 = &type->createStringArrayProperty("prop1");
    });

    // Commit.
    expectNoThrow([this]() { library->commitTypes(); });

    // Check type tables.
    std::vector<utils::Type>     types      = {{0, "type0"}};
    std::vector<utils::Property> properties = {{0, "prop0", alex::toString(alex::DataType::String), 0, 1, 1, 0},
                                               {0, "prop1", alex::toString(alex::DataType::String), 0, 1, 1, 0}};
    std::vector<utils::Member>   members    = {{0, type->getId(), prop0->getId()}, {0, type->getId(), prop1->getId()}};
    checkTypeTables(std::move(types), std::move(properties), std::move(members));
}
