////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria/core/library.h"
#include "alexandria/core/namespace.h"
#include "alexandria/queries/get_query.h"
#include "alexandria/queries/insert_query.h"
#include "alexandria/queries/table_sets.h"
#include "parsertongue/parser.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "geometry/export.h"
#include "geometry/library.h"
#include "geometry/types/cube.h"
#include "geometry/types/float3.h"
#include "geometry/types/material.h"
#include "geometry/types/mesh.h"
#include "geometry/types/node.h"
#include "geometry/types/scene.h"
#include "geometry/types/sphere.h"

#ifdef WIN32
#include "geometry/winutils.h"
#endif

int main(int, char**)
{
    // Set path next to executable.
#ifdef WIN32
    setWinWorkDir();
#endif

    auto                     library      = createLibrary();
    auto&                    mainSpace    = library->getNamespace("main");
    auto&                    typeCube     = mainSpace.getType("cube");
    auto&                    typeMaterial = mainSpace.getType("material");
    auto&                    typeMesh     = mainSpace.getType("mesh");
    auto&                    typeNode     = mainSpace.getType("node");
    auto&                    typeScene    = mainSpace.getType("scene");
    auto&                    typeSphere   = mainSpace.getType("sphere");
    Cube::descriptor_t       descCube(typeCube);
    Material::descriptor_t   descMaterial(typeMaterial);
    Mesh::descriptor_t       descMesh(typeMesh);
    Node::descriptor_t       descNode(typeNode);
    Scene::descriptor_t      descScene(typeScene);
    Sphere::descriptor_t     descSphere(typeSphere);
    auto                     tablesCube     = alex::TableSets(descCube);
    auto                     tablesMaterial = alex::TableSets(descMaterial);
    auto                     tablesMesh     = alex::TableSets(descMesh);
    auto                     tablesNode     = alex::TableSets(descNode);
    auto                     tablesScene    = alex::TableSets(descScene);
    auto                     tablesSphere   = alex::TableSets(descSphere);
    Cube::get_query_t        getCube(descCube);
    Material::get_query_t    getMaterial(descMaterial);
    Mesh::get_query_t        getMesh(descMesh);
    Node::get_query_t        getNode(descNode);
    Scene::get_query_t       getScene(descScene);
    Sphere::get_query_t      getSphere(descSphere);
    Cube::insert_query_t     insertCube(descCube);
    Material::insert_query_t insertMaterial(descMaterial);
    Mesh::insert_query_t     insertMesh(descMesh);
    Node::insert_query_t     insertNode(descNode);
    Scene::insert_query_t    insertScene(descScene);
    Sphere::insert_query_t   insertSphere(descSphere);

    std::unordered_map<std::string, alex::InstanceId> cache;

    pt::parser parser(0, nullptr, true);

    auto flagClear = parser.add_flag('\0', "clear");
    flagClear->set_help(
      "Clear instances in the database.",
      "Clear instances in the database. You can either clear --all instances, or instances of a specific --type.");

    auto flagCreate = parser.add_flag('c', "create");
    flagCreate->set_help("Create a new instance.");

    auto flagDelete = parser.add_flag('d', "delete");
    flagDelete->set_help("Delete an instance.");

    auto flagExport = parser.add_flag('e', "export");
    flagExport->set_help("Export a scene to an OBJ file.");

    auto flagImport = parser.add_flag('i', "import");
    flagImport->set_help("Import meshes from an OBJ file.");

    auto flagInspect = parser.add_flag('\0', "inspect");
    flagInspect->set_help("Inspect an instance.");

    auto flagList = parser.add_flag('l', "list");
    flagList->set_help(
      "List instances in the database.",
      "List instances in the database. You can either list --all instances, or instances of a specific --type.");

    auto flagAll       = parser.add_flag('\0', "all");
    auto flagCache     = parser.add_flag('\0', "cache");
    auto flagCube      = parser.add_flag('\0', "cube");
    auto flagMaterial  = parser.add_flag('\0', "material");
    auto flagMesh      = parser.add_flag('\0', "mesh");
    auto flagNode      = parser.add_flag('\0', "node");
    auto flagScene     = parser.add_flag('\0', "scene");
    auto flagSphere    = parser.add_flag('\0', "sphere");
    auto valueMaterial = parser.add_value<std::string>('\0', "mtl");
    valueMaterial->set_help("Identifier or variable name pointing to a material.");
    auto valueName   = parser.add_value<std::string>('\0', "name");
    auto valueRadius = parser.add_value<float>('\0', "radius");
    auto valueScene  = parser.add_value<std::string>('\0', "sc");
    valueScene->set_help("Identifier or variable name pointing to a scene.");
    auto valueShape = parser.add_value<std::string>('\0', "shape");
    valueShape->set_help("Identifier or variable name pointing to a shape.");
    auto valueSpecular = parser.add_value<float>('\0', "specular");
    auto valueVariable = parser.add_value<std::string>('\0', "variable");
    valueVariable->set_help("Assign an identifier to, or read one from, a named variable.");
    auto listColor = parser.add_list<float>('\0', "color");
    auto listNodes = parser.add_list<std::string>('\0', "nodes");
    auto listSize  = parser.add_list<float>('\0', "size");
    listSize->set_help("List of floating point values.");
    auto listTranslation = parser.add_list<float>('\0', "translation");
    listTranslation->set_help("List of floating point values.");

    std::string line, error;
    while (true)
    {
        std::getline(std::cin, line);
        if (line.empty()) continue;
        if (line == "exit") return 0;
        parser.reset(line, true);
        if (!parser(error))
        {
            std::cout << "Internal parsing error: " << error << std::endl;
            continue;
        }
        if (parser.display_help(std::cout))
        {
            std::cout << std::endl;
            continue;
        }
        if (!parser.get_errors().empty()) parser.display_errors(std::cout);

        try
        {
            if (flagClear->is_set())
            {
                // TODO
            }
            else if (flagCreate->is_set())
            {
                if (flagCube->is_set())
                {
                    if (listSize->get_values().size() < 3) throw std::runtime_error("Need 3 size values.");

                    Cube cube;
                    cube.name = valueName->get_value();
                    cube.size = float3{
                      .x = listSize->get_values()[0], .y = listSize->get_values()[1], .z = listSize->get_values()[2]};
                    insertCube(cube);

                    if (valueVariable->is_set()) cache.insert_or_assign(valueVariable->get_value(), cube.id);

                    std::cout << "Created cube with id=" << cube.id << std::endl;
                }
                else if (flagMaterial->is_set())
                {
                    if (listColor->get_values().size() < 3) throw std::runtime_error("Need 3 color values.");

                    Material material;
                    material.name     = valueName->get_value();
                    material.color    = float3{.x = listColor->get_values()[0],
                                               .y = listColor->get_values()[1],
                                               .z = listColor->get_values()[2]};
                    material.specular = valueSpecular->get_value();
                    insertMaterial(material);

                    if (valueVariable->is_set()) cache.insert_or_assign(valueVariable->get_value(), material.id);

                    std::cout << "Created material with id=" << material.id << std::endl;
                }
                else if (flagNode->is_set())
                {
                    if (listTranslation->get_values().size() < 3)
                        throw std::runtime_error("Need 3 translation values.");

                    Node node;
                    node.name        = valueName->get_value();
                    node.translation = float3{.x = listTranslation->get_values()[0],
                                              .y = listTranslation->get_values()[1],
                                              .z = listTranslation->get_values()[2]};

                    // Retrieve material from cache variable or assign identifier directly.
                    if (auto it = cache.find(valueMaterial->get_value()); it != cache.end())
                        node.material = it->second;
                    else
                        node.material = valueMaterial->get_value();

                    // Retrieve shape from cache variable or assign identifier directly.
                    alex::InstanceId shapeId;
                    if (auto it = cache.find(valueShape->get_value()); it != cache.end())
                        shapeId = it->second;
                    else
                        shapeId = valueShape->get_value();
                    if (flagCube->is_set())
                        node.cube = shapeId;
                    else if (flagMesh->is_set())
                        node.mesh = shapeId;
                    else if (flagSphere->is_set())
                        node.sphere = shapeId;
                    else
                        throw std::runtime_error("Need to specify type of shape.");

                    insertNode(node);

                    std::cout << "Created node with id=" << node.id << std::endl;
                }
                else if (flagScene->is_set())
                {
                    Scene scene;
                    scene.name = valueName->get_value();
                    for (const auto& node : listNodes->get_values())
                    {
                        // Retrieve node from cache variable or assign identifier directly.
                        if (auto it = cache.find(node); it != cache.end())
                            scene.nodes.add(it->second);
                        else
                            scene.nodes.add(alex::InstanceId(node));
                    }
                    insertScene(scene);

                    if (valueVariable->is_set()) cache.insert_or_assign(valueVariable->get_value(), scene.id);

                    std::cout << "Created scene with id=" << scene.id << std::endl;
                }
                else if (flagSphere->is_set())
                {
                    Sphere sphere;
                    sphere.name   = valueName->get_value();
                    sphere.radius = valueRadius->get_value();
                    insertSphere(sphere);

                    if (valueVariable->is_set()) cache.insert_or_assign(valueVariable->get_value(), sphere.id);

                    std::cout << "Created sphere with id=" << sphere.id << std::endl;
                }
                else { std::cout << "Missing appropriate arguments, no new isntances were created" << std::endl; }
            }
            else if (flagDelete->is_set())
            {
                // TODO
            }
            else if (flagExport->is_set())
            {
                Scene scene;
                if (auto it = cache.find(valueScene->get_value()); it != cache.end())
                    scene.id = it->second;
                else
                    scene.id = valueScene->get_value();
                getScene(scene);
                scene.nodes.getQuery = &getNode;

                exportObj(scene, getCube, getMaterial, getMesh, getSphere);
            }
            else if (flagImport->is_set())
            {
                // TODO
            }
            else if (flagList->is_set())
            {
                // Constructs a query that selects the identifier and name of the instance table.
                constexpr auto lister = [](auto&& tableSets) {
                    for (const auto row :
                         tableSets.getInstanceTable()
                           .select(tableSets.getInstanceTable().col<1>(), tableSets.getInstanceTable().col<2>())
                           .compile())
                        std::cout << "name=" << std::get<1>(row) << " id=" << std::get<0>(row) << std::endl;
                };

                if (flagAll->is_set() || flagCache->is_set())
                {
                    std::cout << "Variables:" << std::endl;
                    for (const auto& [name, id] : cache) std::cout << name << "=" << id << std::endl;
                }

                if (flagAll->is_set() || flagCube->is_set())
                {
                    std::cout << "Cubes:" << std::endl;
                    lister(tablesCube);
                }

                if (flagAll->is_set() || flagMaterial->is_set())
                {
                    std::cout << "Materials:" << std::endl;
                    lister(tablesMaterial);
                }

                if (flagAll->is_set() || flagMesh->is_set())
                {
                    std::cout << "Meshes:" << std::endl;
                    lister(tablesMesh);
                }

                if (flagAll->is_set() || flagNode->is_set())
                {
                    std::cout << "Nodes:" << std::endl;
                    lister(tablesNode);
                }

                if (flagAll->is_set() || flagScene->is_set())
                {
                    std::cout << "Scenes:" << std::endl;
                    lister(tablesScene);
                }

                if (flagAll->is_set() || flagSphere->is_set())
                {
                    std::cout << "Spheres:" << std::endl;
                    lister(tablesSphere);
                }
            }
            else if (flagImport->is_set())
            {
                // TODO
            }
            else if (flagInspect->is_set())
            {
                // TODO
            }
        }
        catch (const pt::parser_tongue_exception& e)
        {
            std::cout << e.what() << std::endl;
        }
        catch (const std::exception& e)
        {
            std::cout << e.what() << std::endl;
        }
    }
}
