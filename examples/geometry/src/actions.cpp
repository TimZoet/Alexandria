#include "geometry/actions.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <filesystem>
#include <iostream>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria/queries/query.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "geometry/export.h"
#include "geometry/import.h"

void actionCache(const std::string& param, MaterialHandler& materialHandler, MeshHandler& meshHandler)
{
    if (param == "on" || param == "1" || param == "true")
    {
        materialHandler->setDefaultCacheMethod(alex::CacheMethod::Strong);
        meshHandler->setDefaultCacheMethod(alex::CacheMethod::Strong);
        std::cout << "Enabling caching\n";
    }
    else if (param == "off" || param == "0" || param == "false")
    {
        materialHandler->setDefaultCacheMethod(alex::CacheMethod::None);
        meshHandler->setDefaultCacheMethod(alex::CacheMethod::None);
        std::cout << "Disabling caching\n";
    }
    else
    {
        std::cout << "Unknown caching parameter\n";
    }
}

void actionClear(MaterialHandler& materialHandler, MeshHandler& meshHandler)
{
    materialHandler->clearAll();
    meshHandler->clearAll();
}

void actionExport(const std::string& name, MaterialHandler& materialHandler, MeshHandler& meshHandler)
{
    // Find all meshes whose name matches parameter.
    auto colName = meshHandler->getPrimitiveColumn<1>();
    auto query   = meshHandler->find(colName == name);
    for (const auto id : query())
    {
        // Get mesh and material.
        const auto mesh     = meshHandler->get(id);
        const auto material = mesh->material.get(*materialHandler);

        // Generate unique filename.
        auto   filename = name + ".obj";
        size_t i        = 0;
        while (std::filesystem::exists(filename)) filename = name + "_" + std::to_string(i++) + ".obj";

        // Export mesh and material to OBJ file.
        exportModel(filename, *mesh, *material);
    }
}

void actionImport(const std::string& file, MaterialHandler& materialHandler, MeshHandler& meshHandler)
{
    // Import OBJ file.
    importModel(
      file,
      [&meshHandler](std::shared_ptr<Mesh> m) { meshHandler->insert(std::move(m)); },
      [&materialHandler](std::shared_ptr<Material> m) { materialHandler->insert(std::move(m)); });
}

void actionLink(const std::string& param, MaterialHandler& materialHandler, MeshHandler& meshHandler)
{
    // Split line into two identifiers.
    const auto pos = param.find(' ');
    if (pos == std::string::npos)
    {
        std::cout << "Failed to parse parameters\n";
        return;
    }
    const auto materialID = std::stoull(param.substr(0, pos));
    const auto meshID     = std::stoull(param.substr(pos + 1));

    // Retrieve mesh and material.
    auto       mesh = meshHandler->get(meshID);
    const auto mat  = materialHandler->get(materialID);

    // Link material to mesh and update.
    std::cout << "Linking material " << mat->name << " to mesh " << mesh->name << std::endl;
    mesh->material = *mat;
    meshHandler->update(mesh);
}

void actionList(MaterialHandler& materialHandler, MeshHandler& meshHandler)
{
    std::cout << "---- Meshes ----\n";
    for (const auto id : meshHandler->list())
    {
        const auto mesh = meshHandler->get(id);
        std::cout << mesh->id.get() << " " << mesh->name << "\n";
    }

    std::cout << "---- Materials ----\n";
    for (const auto id : materialHandler->list())
    {
        const auto material = materialHandler->get(id);
        std::cout << material->id.get() << " " << material->name << "\n";
    }
}

void actionOpen(const std::string& file,
                alex::LibraryPtr&  library,
                MaterialHandler&   materialHandler,
                MeshHandler&       meshHandler)
{
    try
    {
        // Reset pointer, closing previously opened library.
        library.reset();

        // Open or create library.
        auto [l, c] = alex::Library::openOrCreate(file);
        library     = std::move(l);

        std::cout << (c ? "Creating new library file " : "Opening existing library file ") << file << std::endl;

        // Library is new, create types.
        if (c)
        {
            auto& float2Type = library->createType("float2");
            float2Type.createPrimitiveProperty("x", alex::DataType::Float);
            float2Type.createPrimitiveProperty("y", alex::DataType::Float);

            auto& float3Type = library->createType("float3");
            float3Type.createPrimitiveProperty("x", alex::DataType::Float);
            float3Type.createPrimitiveProperty("y", alex::DataType::Float);
            float3Type.createPrimitiveProperty("z", alex::DataType::Float);

            auto& vertexType = library->createType("vertex");
            vertexType.createTypeProperty("position", float3Type);
            vertexType.createTypeProperty("normal", float3Type);
            vertexType.createTypeProperty("uv", float2Type);

            auto& materialType = library->createType("material");
            materialType.createStringProperty("name");
            materialType.createTypeProperty("color", float3Type);
            materialType.createPrimitiveProperty("specular", alex::DataType::Float);

            auto& meshType = library->createType("mesh");
            meshType.createStringProperty("name");
            meshType.createBlobProperty("vertices");
            meshType.createReferenceProperty("material", materialType);

            library->commitTypes();
        }

        // Create material handler.
        materialHandler = library->createObjectHandler<alex::Member<&Material::id>,
                                                       alex::Member<&Material::name>,
                                                       alex::NestedMember<float3_t, &Material::color>,
                                                       alex::Member<&Material::specular>>("material");

        // Create mesh handler.
        meshHandler = library->createObjectHandler<alex::Member<&Mesh::id>,
                                                   alex::Member<&Mesh::name>,
                                                   alex::Member<&Mesh::vertices>,
                                                   alex::Member<&Mesh::material>>("mesh");
    }
    catch (const std::exception& e)
    {
        std::cout << "Failed to open or create " << file << ". An unexpected exception occurred: " << e.what()
                  << std::endl;
    }
}

void actionRelease(MaterialHandler& materialHandler, MeshHandler& meshHandler)
{
    materialHandler->releaseAll();
    meshHandler->releaseAll();
}

void actionSubdivide(const std::string& param, MeshHandler& meshHandler)
{
    // Retrieve mesh.
    const auto meshID = std::stoull(param);
    auto       mesh   = meshHandler->get(meshID);

    auto&               vertices = mesh->vertices.get();
    std::vector<Vertex> newVertices;
    newVertices.reserve(vertices.size() * 4);

    for (size_t i = 0; i < vertices.size() / 3; i++)
    {
        auto& v0 = vertices[i * 3 + 0];
        auto& v1 = vertices[i * 3 + 1];
        auto& v2 = vertices[i * 3 + 2];
        auto  v3 = average(v0, v1);
        auto  v4 = average(v1, v2);
        auto  v5 = average(v2, v0);

        newVertices.emplace_back(v0);
        newVertices.emplace_back(v3);
        newVertices.emplace_back(v5);

        newVertices.emplace_back(v3);
        newVertices.emplace_back(v1);
        newVertices.emplace_back(v4);

        newVertices.emplace_back(v4);
        newVertices.emplace_back(v2);
        newVertices.emplace_back(v5);

        newVertices.emplace_back(v3);
        newVertices.emplace_back(v4);
        newVertices.emplace_back(v5);
    }

    vertices = std::move(newVertices);

    // Update.
    meshHandler->update(mesh);
}