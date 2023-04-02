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

#include "alexandria-core/library.h"
#include "alexandria-core/namespace.h"
#include "alexandria-query/delete_query.h"
#include "alexandria-query/get_query.h"
#include "alexandria-query/table_sets.h"
#include "parsertongue/parser.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "geometry/export.h"
#include "geometry/import.h"
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

    // Setup library, types and queries.
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
    Cube::delete_query_t     deleteCube(descCube);
    Material::delete_query_t deleteMaterial(descMaterial);
    Mesh::delete_query_t     deleteMesh(descMesh);
    Node::delete_query_t     deleteNode(descNode);
    Scene::delete_query_t    deleteScene(descScene);
    Sphere::delete_query_t   deleteSphere(descSphere);
    Cube::get_query_t        getCube(descCube);
    Material::get_query_t    getMaterial(descMaterial);
    Mesh::get_query_t        getMesh(descMesh);
    Node::get_query_t        getNode(descNode);
    Scene::get_query_t       getScene(descScene);
    Sphere::get_query_t      getSphere(descSphere);
    CubeInsertQuery          insertCube(descCube);
    MaterialInsertQuery      insertMaterial(descMaterial);
    MeshInsertQuery          insertMesh(descMesh);
    NodeInsertQuery          insertNode(descNode);
    SceneInsertQuery         insertScene(descScene);
    SphereInsertQuery        insertSphere(descSphere);
    Scene::update_query_t    updateScene(descScene);

    std::unordered_map<std::string, alex::InstanceId> cache;

    // Setup parser.
    pt::parser parser(0, nullptr, true);

    auto flagAppend = parser.add_flag('a', "append");
    flagAppend->set_help("Append a node to a scene.");

    auto flagClean = parser.add_flag('\0', "clean");
    flagClean->set_help("Clean instances that are not referenced.");

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

    auto flagUsers = parser.add_flag('\0', "users");
    flagUsers->set_help("Get other objects that reference this instance.");

    auto flagAll      = parser.add_flag('\0', "all");
    auto flagCache    = parser.add_flag('\0', "cache");
    auto flagCube     = parser.add_flag('\0', "cube");
    auto flagMaterial = parser.add_flag('\0', "material");
    auto flagMesh     = parser.add_flag('\0', "mesh");
    auto flagNode     = parser.add_flag('\0', "node");
    auto flagScene    = parser.add_flag('\0', "scene");
    auto flagSphere   = parser.add_flag('\0', "sphere");
    auto valueFile    = parser.add_value<std::string>('\0', "file");
    valueFile->set_help("File to export to.");
    auto valueIdentifier = parser.add_value<std::string>('\0', "identifier");
    valueIdentifier->set_help("Identifier or variable name pointing to an instance.");
    auto valueMaterial = parser.add_value<std::string>('\0', "mtl");
    valueMaterial->set_help("Identifier or variable name pointing to a material.");
    auto valueName     = parser.add_value<std::string>('\0', "name");
    auto valueRadius   = parser.add_value<float>('\0', "radius");
    auto valueSpecular = parser.add_value<float>('\0', "specular");
    auto valueVariable = parser.add_value<std::string>('\0', "variable");
    valueVariable->set_help("Assign an identifier to, or read one from, a named variable.");
    auto listColor = parser.add_list<float>('\0', "color");
    auto listNodes = parser.add_list<std::string>('\0', "nodes");
    auto listSize  = parser.add_list<float>('\0', "size");
    listSize->set_help("List of floating point values.");
    auto listTranslation = parser.add_list<float>('\0', "translation");
    listTranslation->set_help("List of floating point values.");

    // Loop.
    std::string line, error;
    while (true)
    {
        std::cout << "> ";
        std::getline(std::cin, line);
        if (line.empty()) continue;
        if (line == "exit" || line == "quit" || line == "q") return 0;
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
            if (flagAppend->is_set())
            {
                alex::InstanceId sceneId;
                if (auto it = cache.find(valueIdentifier->get_value()); it != cache.end())
                    sceneId = it->second;
                else
                    sceneId = valueIdentifier->get_value();

                Scene scene;
                scene.id = sceneId;
                getScene(scene);

                for (const auto& node : listNodes->get_values())
                {
                    // Retrieve node from cache variable or assign identifier directly.
                    if (auto it = cache.find(node); it != cache.end())
                        scene.nodes.add(it->second);
                    else
                        scene.nodes.add(alex::InstanceId(node));
                }
                updateScene(scene);

                if (valueVariable->is_set()) cache.insert_or_assign(valueVariable->get_value(), scene.id);

                std::cout << "Appended " << listNodes->get_values().size() << " nodes to scene with id=" << scene.id
                          << std::endl;
            }
            else if (flagClean->is_set())
            {
                if (flagCube->is_set() || flagAll->is_set())
                {
                    auto& objectTable     = tablesCube.getInstanceTable();
                    auto& nodeTable       = tablesNode.getInstanceTable();
                    auto  objectIdColumn  = tablesCube.getInstanceColumn<"id">();
                    auto  nodeIdColumn    = tablesNode.getInstanceColumn<"id">();
                    auto  objectRefColumn = tablesNode.getInstanceColumn<"cube">();
                    auto  stmt            = objectTable.join(sql::LeftJoin, nodeTable)
                                  .on(objectRefColumn == objectIdColumn)
                                  .selectAs<std::string>(objectIdColumn)
                                  .where(nodeIdColumn == nullptr)
                                  .compile()
                                  .bind(sql::BindParameters::All);

                    std::cout << "Deleting unused cubes:" << std::endl;
                    for (const auto& id : stmt)
                    {
                        std::cout << id << std::endl;
                        deleteCube(alex::InstanceId(id));
                    }
                }

                if (flagMaterial->is_set() || flagAll->is_set())
                {
                    auto& objectTable     = tablesMaterial.getInstanceTable();
                    auto& nodeTable       = tablesNode.getInstanceTable();
                    auto  objectIdColumn  = tablesMaterial.getInstanceColumn<"id">();
                    auto  nodeIdColumn    = tablesNode.getInstanceColumn<"id">();
                    auto  objectRefColumn = tablesNode.getInstanceColumn<"material">();
                    auto  stmt            = objectTable.join(sql::LeftJoin, nodeTable)
                                  .on(objectRefColumn == objectIdColumn)
                                  .selectAs<std::string>(objectIdColumn)
                                  .where(nodeIdColumn == nullptr)
                                  .compile()
                                  .bind(sql::BindParameters::All);

                    std::cout << "Deleting unused materials:" << std::endl;
                    for (const auto& id : stmt)
                    {
                        std::cout << id << std::endl;
                        deleteMaterial(alex::InstanceId(id));
                    }
                }

                if (flagMesh->is_set() || flagAll->is_set())
                {
                    auto& objectTable     = tablesMesh.getInstanceTable();
                    auto& nodeTable       = tablesNode.getInstanceTable();
                    auto  objectIdColumn  = tablesMesh.getInstanceColumn<"id">();
                    auto  nodeIdColumn    = tablesNode.getInstanceColumn<"id">();
                    auto  objectRefColumn = tablesNode.getInstanceColumn<"mesh">();
                    auto  stmt            = objectTable.join(sql::LeftJoin, nodeTable)
                                  .on(objectRefColumn == objectIdColumn)
                                  .selectAs<std::string>(objectIdColumn)
                                  .where(nodeIdColumn == nullptr)
                                  .compile()
                                  .bind(sql::BindParameters::All);

                    std::cout << "Deleting unused mesh:" << std::endl;
                    for (const auto& id : stmt)
                    {
                        std::cout << id << std::endl;
                        deleteMesh(alex::InstanceId(id));
                    }
                }

                if (flagSphere->is_set() || flagAll->is_set())
                {
                    auto& objectTable     = tablesSphere.getInstanceTable();
                    auto& nodeTable       = tablesNode.getInstanceTable();
                    auto  objectRefColumn = tablesNode.getInstanceColumn<"sphere">();
                    auto  objectIdColumn  = tablesSphere.getInstanceColumn<"id">();
                    auto  nodeIdColumn    = tablesNode.getInstanceColumn<"id">();
                    auto  stmt            = objectTable.join(sql::LeftJoin, nodeTable)
                                  .on(objectRefColumn == objectIdColumn)
                                  .selectAs<std::string>(objectIdColumn)
                                  .where(nodeIdColumn == nullptr)
                                  .compile()
                                  .bind(sql::BindParameters::All);

                    std::cout << "Deleting unused spheres:" << std::endl;
                    for (const auto& id : stmt)
                    {
                        std::cout << id << std::endl;
                        deleteSphere(alex::InstanceId(id));
                    }
                }
            }
            else if (flagCreate->is_set())
            {
                if (flagCube->is_set() && !flagNode->is_set())
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
                    if (auto it = cache.find(valueIdentifier->get_value()); it != cache.end())
                        shapeId = it->second;
                    else
                        shapeId = valueIdentifier->get_value();
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
                else if (flagSphere->is_set() && !flagNode->is_set())
                {
                    Sphere sphere;
                    sphere.name   = valueName->get_value();
                    sphere.radius = valueRadius->get_value();
                    insertSphere(sphere);

                    if (valueVariable->is_set()) cache.insert_or_assign(valueVariable->get_value(), sphere.id);

                    std::cout << "Created sphere with id=" << sphere.id << std::endl;
                }
                else { std::cout << "Missing appropriate arguments, no new instances were created" << std::endl; }
            }
            else if (flagDelete->is_set())
            {
                if (!valueIdentifier->is_set()) throw std::runtime_error("Need an identifier.");
                const alex::InstanceId id(valueIdentifier->get_value());

                if (flagCube->is_set())
                {
                    if (deleteCube(id))
                        std::cout << "Deleted cube with id=" << id << std::endl;
                    else
                        std::cout << "Could not delete cube with id=" << id << std::endl;
                }
                else if (flagMaterial->is_set())
                {
                    if (deleteMaterial(id))
                        std::cout << "Deleted material with id=" << id << std::endl;
                    else
                        std::cout << "Could not delete material with id=" << id << std::endl;
                }
                else if (flagMesh->is_set())
                {
                    if (deleteMesh(id))
                        std::cout << "Deleted mesh with id=" << id << std::endl;
                    else
                        std::cout << "Could not delete mesh with id=" << id << std::endl;
                }
                else if (flagNode->is_set())
                {
                    if (deleteNode(id))
                        std::cout << "Deleted node with id=" << id << std::endl;
                    else
                        std::cout << "Could not delete node with id=" << id << std::endl;
                }
                else if (flagScene->is_set())
                {
                    if (deleteScene(id))
                        std::cout << "Deleted scene with id=" << id << std::endl;
                    else
                        std::cout << "Could not delete scene with id=" << id << std::endl;
                }
                else if (flagSphere->is_set())
                {
                    if (deleteSphere(id))
                        std::cout << "Deleted sphere with id=" << id << std::endl;
                    else
                        std::cout << "Could not delete sphere with id=" << id << std::endl;
                }
                else { std::cout << "Missing appropriate arguments, no instances were deleted" << std::endl; }
            }
            else if (flagExport->is_set())
            {
                Scene scene;
                if (auto it = cache.find(valueIdentifier->get_value()); it != cache.end())
                    scene.id = it->second;
                else
                    scene.id = valueIdentifier->get_value();
                getScene(scene);
                scene.nodes.getQuery = &getNode;

                std::filesystem::path file = valueFile->is_set() ? valueFile->get_value() : "model.obj";
                exportObj(scene, std::move(file), getCube, getMaterial, getMesh, getSphere);
            }
            else if (flagImport->is_set())
            {
                importObj(valueFile->get_value(), insertMaterial, insertMesh, insertNode, insertScene);
            }
            else if (flagInspect->is_set())
            {
                if (!valueIdentifier->is_set()) throw std::runtime_error("Need an identifier.");
                const alex::InstanceId id(valueIdentifier->get_value());

                if (flagCube->is_set())
                {
                    Cube cube;
                    cube.id = id;
                    getCube(cube);
                    std::cout << cube << std::endl;
                }
                else if (flagMaterial->is_set())
                {
                    Material material;
                    material.id = id;
                    getMaterial(material);
                    std::cout << material << std::endl;
                }
                else if (flagMesh->is_set())
                {
                    Mesh mesh;
                    mesh.id = id;
                    getMesh(mesh);
                    std::cout << mesh << std::endl;
                }
                else if (flagNode->is_set())
                {
                    Node node;
                    node.id = id;
                    getNode(node);
                    std::cout << node << std::endl;
                }
                else if (flagScene->is_set())
                {
                    Scene scene;
                    scene.id = id;
                    getScene(scene);
                    std::cout << scene << std::endl;
                }
                else if (flagSphere->is_set())
                {
                    Sphere sphere;
                    sphere.id = id;
                    getSphere(sphere);
                    std::cout << sphere << std::endl;
                }
                else { std::cout << "Missing appropriate arguments, no instances were deleted" << std::endl; }
            }
            else if (flagList->is_set())
            {
                // Constructs a query that selects the identifier and name of the instance table.
                constexpr auto lister = [](auto&& tableSets) {
                    for (const auto row : tableSets.getInstanceTable()
                                            .select(tableSets.getInstanceTable().template col<1>(),
                                                    tableSets.getInstanceTable().template col<2>())
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
            else if (flagUsers->is_set())
            {
                if (!valueIdentifier->is_set()) throw std::runtime_error("Need an identifier.");
                const alex::InstanceId id(valueIdentifier->get_value());

                if (flagCube->is_set())
                {
                    auto& nodeTable       = tablesNode.getInstanceTable();
                    auto& objectTable     = tablesCube.getInstanceTable();
                    auto  objectRefColumn = tablesNode.getInstanceColumn<"cube">();
                    auto  objectIdColumn  = tablesCube.getInstanceColumn<"id">();
                    auto  nodeNameColumn  = tablesNode.getInstanceColumn<"name">();
                    auto  stmt            = nodeTable.join(sql::InnerJoin, objectTable)
                                  .on(objectRefColumn == objectIdColumn)
                                  .selectAs<std::string>(nodeNameColumn)
                                  .where(sql::like(objectIdColumn, id.getAsString()))
                                  .compile()
                                  .bind(sql::BindParameters::All);

                    std::cout << "Nodes referencing cube: \n";
                    for (const auto& name : stmt) std::cout << name << std::endl;
                }
                else if (flagMaterial->is_set())
                {
                    auto& nodeTable       = tablesNode.getInstanceTable();
                    auto& objectTable     = tablesMaterial.getInstanceTable();
                    auto  objectRefColumn = tablesNode.getInstanceColumn<"material">();
                    auto  objectIdColumn  = tablesMaterial.getInstanceColumn<"id">();
                    auto  nodeNameColumn  = tablesNode.getInstanceColumn<"name">();
                    auto  stmt            = nodeTable.join(sql::InnerJoin, objectTable)
                                  .on(objectRefColumn == objectIdColumn)
                                  .selectAs<std::string>(nodeNameColumn)
                                  .where(sql::like(objectIdColumn, id.getAsString()))
                                  .compile()
                                  .bind(sql::BindParameters::All);

                    std::cout << "Nodes referencing material: \n";
                    for (const auto& name : stmt) std::cout << name << std::endl;
                }
                else if (flagMesh->is_set())
                {
                    auto& nodeTable       = tablesNode.getInstanceTable();
                    auto& objectTable     = tablesMesh.getInstanceTable();
                    auto  objectRefColumn = tablesNode.getInstanceColumn<"mesh">();
                    auto  objectIdColumn  = tablesMesh.getInstanceColumn<"id">();
                    auto  nodeNameColumn  = tablesNode.getInstanceColumn<"name">();
                    auto  stmt            = nodeTable.join(sql::InnerJoin, objectTable)
                                  .on(objectRefColumn == objectIdColumn)
                                  .selectAs<std::string>(nodeNameColumn)
                                  .where(sql::like(objectIdColumn, id.getAsString()))
                                  .compile()
                                  .bind(sql::BindParameters::All);

                    std::cout << "Nodes referencing mesh: \n";
                    for (const auto& name : stmt) std::cout << name << std::endl;
                }
                else if (flagNode->is_set())
                {
                    auto& sceneTable      = tablesScene.getInstanceTable();
                    auto& nodeTable       = tablesNode.getInstanceTable();
                    auto& nodeArrayTable  = tablesScene.getReferenceArrayTable<"nodes">();
                    auto  sceneRefColumn  = nodeArrayTable.col<1>();
                    auto  nodeRefColumn   = nodeArrayTable.col<2>();
                    auto  sceneIdColumn   = tablesScene.getInstanceColumn<"id">();
                    auto  sceneNameColumn = tablesScene.getInstanceColumn<"name">();
                    auto  nodeIdColumn    = tablesNode.getInstanceColumn<"id">();
                    auto  stmt            = sceneTable.join(sql::InnerJoin, nodeArrayTable)
                                  .on(sceneRefColumn == sceneIdColumn)
                                  .join(sql::InnerJoin, nodeTable)
                                  .on(nodeRefColumn == nodeIdColumn)
                                  .selectAs<std::string>(sceneNameColumn)
                                  .where(sql::like(nodeIdColumn, id.getAsString()))
                                  .groupBy(sceneIdColumn)
                                  .compile()
                                  .bind(sql::BindParameters::All);

                    std::cout << "Scenes referencing node: \n";
                    for (const auto& name : stmt) std::cout << name << std::endl;
                }
                else if (flagSphere->is_set())
                {
                    auto& nodeTable       = tablesNode.getInstanceTable();
                    auto& objectTable     = tablesSphere.getInstanceTable();
                    auto  objectRefColumn = tablesNode.getInstanceColumn<"sphere">();
                    auto  objectIdColumn  = tablesSphere.getInstanceColumn<"id">();
                    auto  nodeNameColumn  = tablesNode.getInstanceColumn<"name">();
                    auto  stmt            = nodeTable.join(sql::InnerJoin, objectTable)
                                  .on(objectRefColumn == objectIdColumn)
                                  .selectAs<std::string>(nodeNameColumn)
                                  .where(sql::like(objectIdColumn, id.getAsString()))
                                  .compile()
                                  .bind(sql::BindParameters::All);

                    std::cout << "Nodes referencing sphere: \n";
                    for (const auto& name : stmt) std::cout << name << std::endl;
                }
                else { std::cout << "Missing appropriate arguments" << std::endl; }
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
