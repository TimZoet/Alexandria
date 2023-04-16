#include "geometry/library.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria-core/library.h"
#include "alexandria-core/namespace.h"
#include "alexandria-core/type.h"
#include "alexandria-core/type_layout.h"

alex::LibraryPtr createLibrary()
{
    auto [library, created] = alex::Library::openOrCreate("geometry.db");
    if (created)
    {
        auto& mainSpace = library->createNamespace("main");

        alex::TypeLayout layoutFloat3;
        layoutFloat3.createPrimitiveProperty("x", alex::DataType::Float);
        layoutFloat3.createPrimitiveProperty("y", alex::DataType::Float);
        layoutFloat3.createPrimitiveProperty("z", alex::DataType::Float);
        layoutFloat3.commit(mainSpace, "float3", alex::TypeLayout::Instantiable::False);
        alex::Type& typeFloat3 = mainSpace.getType("float3");

        alex::TypeLayout layoutCube;
        layoutCube.createStringProperty("name");
        layoutCube.createNestedTypeProperty("size", typeFloat3);
        layoutCube.commit(mainSpace, "cube");
        alex::Type& typeCube = mainSpace.getType("cube");

        alex::TypeLayout layoutMaterial;
        layoutMaterial.createStringProperty("name");
        layoutMaterial.createNestedTypeProperty("color", typeFloat3);
        layoutMaterial.createPrimitiveProperty("specular", alex::DataType::Float);
        layoutMaterial.commit(mainSpace, "material");
        alex::Type& typeMaterial = mainSpace.getType("material");

        alex::TypeLayout layoutMesh;
        layoutMesh.createStringProperty("name");
        layoutMesh.createBlobProperty("vertices");
        layoutMesh.createBlobProperty("indices");
        layoutMesh.commit(mainSpace, "mesh");
        alex::Type& typeMesh = mainSpace.getType("mesh");

        alex::TypeLayout layoutSphere;
        layoutSphere.createStringProperty("name");
        layoutSphere.createPrimitiveProperty("radius", alex::DataType::Float);
        layoutSphere.commit(mainSpace, "sphere");
        alex::Type& typeSphere = mainSpace.getType("sphere");

        alex::TypeLayout layoutNode;
        layoutNode.createStringProperty("name");
        layoutNode.createNestedTypeProperty("translation", typeFloat3);
        layoutNode.createReferenceProperty("material", typeMaterial);
        layoutNode.createReferenceProperty("cube", typeCube);
        layoutNode.createReferenceProperty("mesh", typeMesh);
        layoutNode.createReferenceProperty("sphere", typeSphere);
        layoutNode.commit(mainSpace, "node");
        alex::Type& typeNode = mainSpace.getType("node");

        alex::TypeLayout layoutScene;
        layoutScene.createStringProperty("name");
        layoutScene.createReferenceArrayProperty("nodes", typeNode);
        layoutScene.commit(mainSpace, "scene");
    }

    return std::move(library);
}
