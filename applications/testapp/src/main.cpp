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
    alex::Type *type0;
    if (created)
    {
        type0 = &library->createType("type0");
        //type1            = &library->createType("type1");

        auto& blobProp = library->createBlobProperty("blob", false);
        auto& floatProp = library->createPrimitiveProperty("float", alex::DataType::Float);
        type0->addProperty(blobProp);
        type0->addProperty(floatProp);
        library->commitTypes();
    }
    else
    {
        type0 = &library->getType("type0");
        //type1 = &library->getType("type1");
    }

    auto handler0 = library->createObjectHandler<&Type0::id, &Type0::blob, &Type0::f>(type0->getName());

    //auto handler1 = library->createObjectHandler<&Type1::id, &Type1::blob>(type1->getName());



    Type0 obj0;
    obj0.blob.get().push_back(10);
    obj0.blob.get().push_back(11);
    obj0.blob.get().push_back(12);
    obj0.blob.get().push_back(13);
    handler0.insert(obj0);

    /*Type1 obj1;
    obj1.blob.get().push_back(11);
    obj1.blob.get().push_back(22);
    obj1.blob.get().push_back(33);
    handler1.insert(obj1);*/

    //auto x = t0.get(2);
    std::cout << 'x';
}
