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
                materialHandler->setDefaultCacheMethod(alex::CacheMethod::Strong);

                meshHandler = library->createObjectHandler<alex::Member<&Mesh::id>,
                                                           alex::Member<&Mesh::name>,
                                                           alex::Member<&Mesh::vertices>,
                                                           alex::Member<&Mesh::material>>("mesh");
                meshHandler->setDefaultCacheMethod(alex::CacheMethod::Strong);
            }
            catch (const std::exception& e)
            {
                std::cout << "Failed to open or create " << line << ". An unexpected exception occurred: " << e.what()
                          << std::endl;
            }
        }
        else if (line == "list")
        {
        }
        else if (line.starts_with("import "))
        {
            line.erase(0, 7);

            importModel(
              line,
              [&meshHandler]() -> std::shared_ptr<Mesh> { return meshHandler->create(); },
              [&materialHandler]() -> std::shared_ptr<Material> { return materialHandler->create(); });
        }
        else if (line.starts_with("export "))
        {
            line.erase(0, 7);
        }
        else
        {
            std::cout << "Unknown command" << std::endl;
        }
    }
    return 0;
}
