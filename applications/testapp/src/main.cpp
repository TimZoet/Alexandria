#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>

#include "alexandria/library.h"

int main()
{
    const auto dbPath = std::filesystem::current_path() / "file.db";
    std::filesystem::remove(dbPath);

    {
        
#if 1
        auto library = alex::Library::create(dbPath);
        auto& float3_t   = library->createType("float3");
        auto& material_t = library->createType("material");
        auto& mesh_t     = library->createType("mesh");
        auto& mat3x3_t   = library->createType("mat3x3");
        auto& node_t     = library->createType("node");

        auto& prop_x         = library->createPrimitiveProperty("x", alex::DataType::Float);
        auto& prop_y         = library->createPrimitiveProperty("y", alex::DataType::Float);
        auto& prop_z         = library->createPrimitiveProperty("z", alex::DataType::Float);
        auto& prop_name      = library->createPrimitiveProperty("name", alex::DataType::String);
        auto& prop_albedo    = library->createNestedProperty("albedo", float3_t);
        auto& prop_specular  = library->createPrimitiveProperty("specular", alex::DataType::Float);
        auto& prop_vertices  = library->createBlobProperty("vertices");
        auto& prop_r1        = library->createNestedProperty("r1", float3_t);
        auto& prop_r2        = library->createNestedProperty("r2", float3_t);
        auto& prop_r3        = library->createNestedProperty("r3", float3_t);
        auto& prop_transform = library->createNestedProperty("transform", mat3x3_t);
        auto& prop_children  = library->createNestedArrayProperty("children", node_t);
        auto& prop_meshes    = library->createNestedArrayProperty("meshes", mesh_t);
        auto& prop_material  = library->createNestedProperty("material", material_t);

        float3_t.addProperty(prop_x);
        float3_t.addProperty(prop_y);
        float3_t.addProperty(prop_z);
        material_t.addProperty(prop_name);
        material_t.addProperty(prop_albedo);
        material_t.addProperty(prop_specular);
        mesh_t.addProperty(prop_name);
        mesh_t.addProperty(prop_vertices);
        mat3x3_t.addProperty(prop_r1);
        mat3x3_t.addProperty(prop_r2);
        mat3x3_t.addProperty( prop_r3);
        node_t.addProperty(prop_name);
        node_t.addProperty(prop_transform);
        node_t.addProperty(prop_children);
        node_t.addProperty(prop_meshes);
        node_t.addPropertyReference(prop_material);

        library->commitTypes();
#else
        auto library = alex::Library::open(dbPath);
#endif
        std::ofstream graph(std::filesystem::current_path() / "graph.dot");
        library->writeGraph(graph);
    }

    //std::filesystem::remove(dbPath);
}
