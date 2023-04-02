#include "geometry/library.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria-core/library.h"
#include "alexandria-core/namespace.h"
#include "alexandria-core/type.h"

alex::LibraryPtr createLibrary()
{
    auto [library, created] = alex::Library::openOrCreate("geometry.db");
    if (created)
    {
        auto& mainSpace = library->createNamespace("main");

        alex::Type& typeFloat3 = mainSpace.createType("float3", false);
        typeFloat3.createPrimitiveProperty("x", alex::DataType::Float);
        typeFloat3.createPrimitiveProperty("y", alex::DataType::Float);
        typeFloat3.createPrimitiveProperty("z", alex::DataType::Float);
        typeFloat3.commit();

        alex::Type& typeCube = mainSpace.createType("cube");
        typeCube.createStringProperty("name");
        typeCube.createNestedTypeProperty("size", typeFloat3);
        typeCube.commit();

        alex::Type& typeMaterial = mainSpace.createType("material");
        typeMaterial.createStringProperty("name");
        typeMaterial.createNestedTypeProperty("color", typeFloat3);
        typeMaterial.createPrimitiveProperty("specular", alex::DataType::Float);
        typeMaterial.commit();

        alex::Type& typeMesh = mainSpace.createType("mesh");
        typeMesh.createStringProperty("name");
        typeMesh.createBlobProperty("vertices");
        typeMesh.createBlobProperty("indices");
        typeMesh.commit();

        alex::Type& typeSphere = mainSpace.createType("sphere");
        typeSphere.createStringProperty("name");
        typeSphere.createPrimitiveProperty("radius", alex::DataType::Float);
        typeSphere.commit();

        alex::Type& typeNode = mainSpace.createType("node");
        typeNode.createStringProperty("name");
        typeNode.createNestedTypeProperty("translation", typeFloat3);
        typeNode.createReferenceProperty("material", typeMaterial);
        typeNode.createReferenceProperty("cube", typeCube);
        typeNode.createReferenceProperty("mesh", typeMesh);
        typeNode.createReferenceProperty("sphere", typeSphere);
        typeNode.commit();

        alex::Type& typeScene = mainSpace.createType("scene");
        typeScene.createStringProperty("name");
        typeScene.createReferenceArrayProperty("nodes", typeNode);
        typeScene.commit();
    }

    return std::move(library);
}
