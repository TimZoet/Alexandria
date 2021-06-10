////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <filesystem>
#include <iostream>
#include <string>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria/library.h"
#include "alexandria/queries/query.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "geometry/export.h"
#include "geometry/import.h"
#include "geometry/types.h"

#ifdef WIN32
#include "geometry/winutils.h"
#endif

using float2_t = alex::MemberList<alex::Member<&float2::x>, alex::Member<&float2::y>>;
using float3_t = alex::MemberList<alex::Member<&float3::x>, alex::Member<&float3::y>, alex::Member<&float3::z>>;
using vertex_t = alex::MemberList<alex::NestedMember<float3_t, &Vertex::position>,
                                  alex::NestedMember<float3_t, &Vertex::normal>,
                                  alex::NestedMember<float2_t, &Vertex::uv>>;

using MaterialHandler =
  decltype(std::declval<alex::LibraryPtr>()
             ->createObjectHandler<alex::Member<&Material::id>,
                                   alex::Member<&Material::name>,
                                   alex::NestedMember<float3_t, &Material::color>,
                                   alex::Member<&Material::specular>>(std::declval<std::string>()));

using MeshHandler = decltype(std::declval<alex::LibraryPtr>()
                               ->createObjectHandler<alex::Member<&Mesh::id>,
                                                     alex::Member<&Mesh::name>,
                                                     alex::Member<&Mesh::vertices>,
                                                     alex::Member<&Mesh::material>>(std::declval<std::string>()));

int main(int, char**)
{
    // Set path next to executable.
#ifdef WIN32
    setWinWorkDir();
#endif



    alex::LibraryPtr library;
    MaterialHandler  materialHandler;
    MeshHandler      meshHandler;

    std::string line;
    while (true)
    {
        std::getline(std::cin, line);

        if (line.empty()) continue;
        if (line == "exit") return 0;

        if (line.starts_with("open "))
        {
            line.erase(0, 5);
            try
            {
                library.reset();

                auto [l, c] = alex::Library::openOrCreate(line);
                library     = std::move(l);

                std::cout << (c ? "Creating new library file\n" : "Opening existing library file\n");

                // Create types.
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

                // Create object handlers.

                materialHandler = library->createObjectHandler<alex::Member<&Material::id>,
                                                               alex::Member<&Material::name>,
                                                               alex::NestedMember<float3_t, &Material::color>,
                                                               alex::Member<&Material::specular>>("material");

                meshHandler = library->createObjectHandler<alex::Member<&Mesh::id>,
                                                           alex::Member<&Mesh::name>,
                                                           alex::Member<&Mesh::vertices>,
                                                           alex::Member<&Mesh::material>>("mesh");
            }
            catch (const std::exception& e)
            {
                std::cout << "Failed to open or create " << line << ". An unexpected exception occurred: " << e.what()
                          << std::endl;
            }
        }
        else if (line == "list")
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
        else if (line.starts_with("import "))
        {
            line.erase(0, 7);

            // Import OBJ file.
            importModel(
              line,
              [&meshHandler](std::shared_ptr<Mesh> m) { meshHandler->insert(std::move(m)); },
              [&materialHandler](std::shared_ptr<Material> m) { materialHandler->insert(std::move(m)); });
        }
        else if (line.starts_with("export "))
        {
            line.erase(0, 7);

            // Find all meshes whose name matches parameter.
            auto colName = meshHandler->getPrimitiveColumn<1>();
            auto query   = meshHandler->find(colName == line);
            for (const auto id : query())
            {
                // Get mesh and material.
                const auto mesh     = meshHandler->get(id);
                const auto material = mesh->material.get(*materialHandler);

                // Generate unique filename.
                auto   filename = line + ".obj";
                size_t i        = 0;
                while (std::filesystem::exists(filename)) filename = line + "_" + std::to_string(i++) + ".obj";

                // Export mesh and material to OBJ file.
                exportModel(filename, *mesh, *material);
            }
        }
        else if (line.starts_with("link "))
        {
            line.erase(0, 5);

            // Split line into two identifiers.
            const auto pos = line.find(' ');
            if (pos == std::string::npos)
            {
                std::cout << "Failed to parse parameters\n";
                continue;
            }
            const auto materialID = std::stoull(line.substr(0, pos));
            const auto meshID     = std::stoull(line.substr(pos + 1));

            // Retrieve mesh and material.
            auto mesh = meshHandler->get(meshID);
            auto mat  = materialHandler->get(materialID);

            // Link material to mesh and update.
            std::cout << "Linking material " << mat->name << " to mesh " << mesh->name << std::endl;
            mesh->material = *mat;
            meshHandler->update(mesh);
        }
        else if (line.starts_with("cache "))
        {
            line.erase(0, 6);
            if (line == "on" || line == "1" || line == "true")
            {
                meshHandler->setDefaultCacheMethod(alex::CacheMethod::Strong);
                std::cout << "Enabling caching\n";
            }
            else if (line == "off" || line == "0" || line == "false")
            {
                meshHandler->setDefaultCacheMethod(alex::CacheMethod::Strong);
                std::cout << "Disabling caching\n";
            }
            else
            {
                std::cout << "Unknown caching parameter\n";
            }
        }
        else if (line.starts_with("release "))
        {
            std::cout << "Releasing all cached objects\n";
            meshHandler->releaseAll();
            materialHandler->releaseAll();
        }
        else if (line.starts_with("clear "))
        {
            std::cout << "Clearing all cached objects\n";
            meshHandler->clearAll();
        }
        else
        {
            std::cout << "Unknown command" << std::endl;
        }
    }

    return 0;
}
