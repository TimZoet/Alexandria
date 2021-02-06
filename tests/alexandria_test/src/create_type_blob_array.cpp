#include "alexandria_test/create_type_blob_array.h"

void CreateTypeBlobArray::operator()()
{
    // Create type and property.
    alex::Type*     type;
    alex::Property* prop;
    expectNoThrow([&type, this]() { type = &library->createType("type0"); });
    expectNoThrow([&prop, this]() { prop = &library->createBlobProperty("prop0", true); });
    expectNoThrow([&type, &prop]() { type->addProperty(*prop); });

    // Commit.
    expectNoThrow([this]() { library->commitTypes(); });

    // Check type tables.
    std::vector<utils::Type>     types      = {{0, "type0"}};
    std::vector<utils::Property> properties = {{0, "prop0", alex::toString(alex::DataType::Blob), 0, true, 1, 1}};
    std::vector<utils::Member>   members    = {{0, type->getId(), prop->getId()}};
    checkTypeTables(std::move(types), std::move(properties), std::move(members));
}
