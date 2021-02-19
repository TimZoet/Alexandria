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
        type0            = &library->createType("type0");
        type1            = &library->createType("type1");
        auto& propA      = library->createPrimitiveArrayProperty("array", alex::DataType::Float, true);
        auto& propNested = library->createNestedProperty("propNested", *type0, true);
        type0->addProperty(propA);
        type1->addProperty(propNested);
        library->commitTypes();
    }
    else
    {
        type0 = &library->getType("type0");
        type1 = &library->getType("type1");
    }

    auto t0 = library->createObjectHandler<Type0, &Type0::id, &Type0::array>(type0->getName());
    auto t1 = library->createObjectHandler<Type1, &Type1::id, &Type1::type0>(type1->getName());



    Type0 obj0;
    Type0 obj1;
    obj0.array.get().push_back(10);
    obj0.array.get().push_back(20);
    obj0.array.get().push_back(30);
    obj1.array.get().push_back(16);
    obj1.array.get().push_back(32);
    obj1.array.get().push_back(64);

    t0.insert(obj0);
    t0.insert(obj1);

    Type1 obj2;
    Type1 obj3;
    obj2.type0 = obj0;
    obj3.type0 = obj2.type0;

    t1.insert(obj2);
    t1.insert(obj3);

    auto x = t1.get(2);
    std::cout << 'x';
}
