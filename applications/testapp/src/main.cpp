#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>

#include "alexandria/library.h"


#include "testapp/types.h"

int main()
{
    const auto dbPath = std::filesystem::current_path() / "file.db";
    std::filesystem::remove(dbPath);

    auto [library, created] = alex::Library::openOrCreate(dbPath);
    alex::Type *type0, *type1;
    if (created)
    {
        type0 = &library->createType("type0");
        type1 = &library->createType("type1");

        auto& floatProp = library->createPrimitiveProperty("float", alex::DataType::Float);
        auto& intProp   = library->createPrimitiveProperty("int", alex::DataType::Int32);
        auto& blobProp   = library->createBlobProperty("blob", false);
        auto& refProp   = library->createNestedArrayProperty("ref", *type0);
        type0->addProperty(floatProp);
        type0->addProperty(intProp);
        type0->addProperty(blobProp);
        type1->addProperty(refProp);
        library->commitTypes();
    }
    else
    {
        type0 = &library->getType("type0");
        type1 = &library->getType("type1");
    }

    auto handler0 = library->createObjectHandler<&Type0::id, &Type0::x, &Type0::y, &Type0::foo>(type0->getName());
    auto handler1 = library->createObjectHandler<&Type1::id, &Type1::refs>(type1->getName());

    Type0 obj0;
    obj0.x = 5.5f;
    obj0.y = 10;
    obj0.foo.get().emplace_back(3, 4);
    handler0.insert(obj0);

    Type0 obj1;
    obj1.x = 3.5f;
    obj1.y = 33;
    handler0.insert(obj1);

    Type1 obj2;
    obj2.refs.add(obj0);
    obj2.refs.add(obj1);
    handler1.insert(obj2);

    auto x = handler0.get(obj0.id);
    std::cout << 'x';
}
