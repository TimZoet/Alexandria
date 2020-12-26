#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <bit>
#include "cppql/binding.h"
#include "cppql/column.h"
#include "cppql/database.h"
#include "cppql/statement.h"
#include "cppql/table.h"

#include "alexandria/library.h"

int main()
{
    const auto dbPath = std::filesystem::current_path() / "file.db";
    std::filesystem::remove(dbPath);

    {
        auto library = alex::Library::create(dbPath);

        auto& prop_x = library->createProperty("x", alex::Property::DataType::Float, false, false);
        auto& prop_y = library->createProperty("y", alex::Property::DataType::Float, false, false);
        auto& prop_z = library->createProperty("z", alex::Property::DataType::Float, false, false);
        auto& float3_t = library->createType("float3", { &prop_x, &prop_y, &prop_z });
        auto& prop_name = library->createProperty("name", alex::Property::DataType::String, false, false);
        auto& prop_albedo = library->createProperty("albedo", float3_t, false, false);
        auto& prop_specular = library->createProperty("specular", alex::Property::DataType::Float, false, false);
        auto& prop_vertices = library->createProperty("vertices", float3_t, true, true);
        auto& material_t = library->createType("material", { &prop_name, &prop_albedo, &prop_specular });
        auto& mesh_t = library->createType("mesh", { &prop_name, &prop_vertices });

        std::ofstream graph(std::filesystem::current_path() / "graph.dot");
        library->writeGraph(graph);
    }

    //std::filesystem::remove(dbPath);
}
