#include "geometry/import.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <iostream>

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "geometry/tiny_obj_loader.h"

bool importModel(const std::filesystem::path&                          path,
                 const std::function<void(std::shared_ptr<Mesh>)>&     insertMeshFunc,
                 const std::function<void(std::shared_ptr<Material>)>& insertMaterialFunc)
{
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
        // Create new material.
        auto material      = std::make_shared<Material>();
        material->name     = mat.name;
        material->color    = float3{.x = mat.diffuse[0], .y = mat.diffuse[1], .z = mat.diffuse[2]};
        material->specular = mat.specular[0];

        insertMaterialFunc(material);

        materialList.push_back(material->id);
    }

    // Load all shapes and create a mesh for each.
    for (const auto& shape : shapes)
    {
        // Create new mesh.
        auto mesh  = std::make_shared<Mesh>();
        mesh->name = shape.name;

        // Get material from first triangle.
        mesh->material = materialList[static_cast<size_t>(shape.mesh.material_ids[0])];

        auto& vertices = mesh->vertices.get();
        vertices.reserve(shape.mesh.num_face_vertices.size() * 3);

        // Get all vertices.
        for (size_t i = 0; i < shape.mesh.num_face_vertices.size() * 3; i++)
        {
            const auto& idx = shape.mesh.indices[i];

            Vertex v;

            // Get vertex position.
            v.position = float3{.x = attrib.vertices[static_cast<size_t>(idx.vertex_index) * 3 + 0],
                                .y = attrib.vertices[static_cast<size_t>(idx.vertex_index) * 3 + 1],
                                .z = attrib.vertices[static_cast<size_t>(idx.vertex_index) * 3 + 2]};

            // Get vertex normal.
            if (idx.normal_index >= 0)
                v.normal = float3{.x = attrib.normals[static_cast<size_t>(idx.normal_index) * 3 + 0],
                                  .y = attrib.normals[static_cast<size_t>(idx.normal_index) * 3 + 1],
                                  .z = attrib.normals[static_cast<size_t>(idx.normal_index) * 3 + 2]};

            // Get vertex UV coords.
            if (idx.texcoord_index >= 0)
                v.uv = float2{.x = attrib.texcoords[static_cast<size_t>(idx.texcoord_index) * 2 + 0],
                              .y = attrib.texcoords[static_cast<size_t>(idx.texcoord_index) * 2 + 1]};

            vertices.push_back(v);
        }

        insertMeshFunc(std::move(mesh));
    }

    return true;
}