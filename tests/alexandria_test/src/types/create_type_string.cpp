#include "alexandria_test/types/create_type_string.h"

void CreateTypeString::operator()()
{
    // Create type and property.
    alex::Type*     type;
    alex::Property* prop0;
    expectNoThrow([&type, &prop0, this]() {
        type  = &library->createType("type0");
        prop0 = &type->createStringProperty("prop0");
    });

    // Commit.
    expectNoThrow([this]() { library->commitTypes(); });

    // Check type tables.
    std::vector<utils::Type>     types      = {{0, "type0"}};
    std::vector<utils::Property> properties = {{0, "prop0", alex::toString(alex::DataType::String), 0, 0, 0, 0}};
    std::vector<utils::Member>   members    = {{0, type->getId(), prop0->getId()}};
    checkTypeTables(std::move(types), std::move(properties), std::move(members));
}
