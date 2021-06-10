#include "geometry/export.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <fstream>
#include <iostream>

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#define TINYOBJLOADER_IMPLEMENTATION
#include "geometry/tiny_obj_loader.h"

bool exportModel(std::filesystem::path path, const Mesh& mesh, const Material& mtl)
{
    // Export mesh.
    {
        std::ofstream file(path);
        if (!file)
        {
            std::cout << "Could not open " << path << std::endl;
            return false;
        }

        std::cout << "Writing mesh " << mesh.name << " to " << path << std::endl;

        // Write material filename.
        file << "mtllib " << path.replace_extension(".mtl").string() << "\n";

        // Write mesh name.
        file << "o " << mesh.name << "\n";

        // Write vertices.
        for (const auto& v : mesh.vertices.get())
            file << "v " << v.position.x << " " << v.position.y << " " << v.position.z << "\n";

        // Write normals.
        for (const auto& v : mesh.vertices.get())
            file << "vn " << v.normal.x << " " << v.normal.y << " " << v.normal.z << "\n";

        // Write UVs.
        for (const auto& v : mesh.vertices.get()) { file << "vt " << v.uv.x << " " << v.uv.y << "\n"; }

        // Write material.
        file << "usemtl " << mtl.name << "\n";
        file << "s off\n";

        // Write faces.
        for (size_t i = 0; i < mesh.vertices.get().size() / 3; i++)
        {
            file << "f ";
            for (size_t j = 1; j < 4; j++) file << i * 3 + j << "/" << i * 3 + j << "/" << i * 3 + j << "/ ";
            file << "\n";
        }
    }

    // Export material.
    {
        std::ofstream file(path.replace_extension(".mtl"));
        if (!file)
        {
            std::cout << "Could not open " << path << std::endl;
            return false;
        }

        std::cout << "Writing material " << mtl.name << " to " << path << std::endl;

        // Write properties.
        file << "newmtl " << mtl.name << "\n";
        file << "Kd " << mtl.color.x << " " << mtl.color.y << " " << mtl.color.z << "\n";
        file << "Ks " << mtl.specular << " " << mtl.specular << " " << mtl.specular << "\n";
        file << "illum 2\n";
    }

    return true;
}