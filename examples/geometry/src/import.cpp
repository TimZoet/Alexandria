#include "geometry/import.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <iostream>

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#define TINYOBJLOADER_IMPLEMENTATION
#include "geometry/tiny_obj_loader.h"

bool importObj(const std::filesystem::path& path,
               MaterialInsertQuery&         insertMaterial,
               MeshInsertQuery&             insertMesh,
               NodeInsertQuery&             insertNode,
               SceneInsertQuery&            insertScene)
{
    std::cout << "Importing " << path << '\n';

    // Try to load obj file.
    auto                             inputfile = path.string();
    tinyobj::attrib_t                attrib;
    std::vector<tinyobj::shape_t>    shapes;
    std::vector<tinyobj::material_t> materials;
    std::string                      warn;
    std::string                      err;
    auto                             ret = LoadObj(&attrib, &shapes, &materials, &warn, &err, inputfile.c_str());

    //
    if (!warn.empty()) { std::cout << warn << std::endl; }
    if (!err.empty()) { std::cout << err << std::endl; }
    if (!ret) { return false; }

    // Load all materials.
    std::vector<alex::InstanceId> materialList;
    for (const auto& mat : materials)
    {
        Material material;
        material.name     = mat.name;
        material.color    = float3{.x = mat.diffuse[0], .y = mat.diffuse[1], .z = mat.diffuse[2]};
        material.specular = mat.specular[0];

        insertMaterial(material);

        std::cout << "  Created material " << material.id << '\n';

        materialList.push_back(material.id);
    }

    Scene scene;
    scene.name = path.stem().string();

    // Load all shapes and create a mesh and node for each.
    for (const auto& shape : shapes)
    {
        Mesh mesh;
        mesh.name = shape.name;

        // While there is a global vertex array shared by all shapes,
        // we just extract the vertices used by this particular shape.
        // Obviously, this can cause quite a bit of duplication of data.
        auto& vertices = mesh.vertices.get();
        for (auto [vertex_index, normal_index, texcoord_index] : shape.mesh.indices)
        {
            vertices.emplace_back(attrib.vertices[static_cast<size_t>(vertex_index) * 3 + 0],
                                  attrib.vertices[static_cast<size_t>(vertex_index) * 3 + 1],
                                  attrib.vertices[static_cast<size_t>(vertex_index) * 3 + 2]);
        }

        auto& indices = mesh.indices.get();
        for (int32_t i = 0; i < shape.mesh.indices.size() / 3; i++) indices.emplace_back(i * 3, i * 3 + 1, i * 3 + 2);

        insertMesh(mesh);

        std::cout << "  Created mesh " << mesh.id << '\n';

        Node node;
        node.name = shape.name;
        // Get material from first triangle.
        node.material = materialList[static_cast<size_t>(shape.mesh.material_ids[0])];
        node.mesh     = mesh;
        insertNode(node);

        std::cout << "  Created node " << node.id << '\n';

        scene.nodes.add(node);
    }

    insertScene(scene);

    std::cout << "Created scene " << scene.id << '\n';

    return true;
}