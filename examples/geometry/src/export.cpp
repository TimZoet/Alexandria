#include "geometry/export.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <fstream>
#include <numbers>
#include <unordered_set>

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

// TODO: Try and get rid of these query parameters.
void exportObj(const Scene&           scene,
               Cube::get_query_t&     getCube,
               Material::get_query_t& getMaterial,
               Mesh::get_query_t&     getMesh,
               Sphere::get_query_t&   getSphere)
{
    auto file    = std::ofstream("model.obj");
    auto mtlfile = std::ofstream("model.mtl");
    file << "mtllib model.mtl\n";

    const auto writeVertex = [&](const float x, const float y, const float z) {
        file << "v " << x << " " << y << " " << z << "\n";
    };
    const auto writeFace = [&](const int32_t x, const int32_t y, const int32_t z) {
        file << "f " << x << " " << y << " " << z << "\n";
    };
    int32_t vertexIndex = 1;

    std::unordered_set<alex::InstanceId> exportedMtls;

    // Export default purple material.
    mtlfile << "newmtl default\n";
    mtlfile << "Kd 1 0 1\n";
    mtlfile << "Ks 0 0 0\n";
    mtlfile << "Ns 10\n";

    for (const auto& node : scene.nodes.getAll())
    {
        if (!exportedMtls.contains(node.material.getId()))
        {
            Material mtl;
            mtl.id = node.material.getId();
            getMaterial(mtl);

            mtlfile << "newmtl " << mtl.id << '\n';
            mtlfile << "Kd " << mtl.color.x << ' ' << mtl.color.y << ' ' << mtl.color.z << '\n';
            mtlfile << "Ks " << mtl.specular << ' ' << mtl.specular << ' ' << mtl.specular << '\n';
            mtlfile << "Ns 10" << '\n';

            exportedMtls.emplace(node.material.getId());
        }

        // Use default material if there is none or reference material.
        if (node.material.isNone())
            file << "usemtl default\n";
        else
            file << "usemtl " << node.material.getId() << '\n';

        if (!node.cube.isNone())
        {
            Cube cube;
            cube.id = node.cube.getId();
            getCube(cube);

            // Write 8 corner vertices.
            writeVertex(
              node.translation.x - cube.size.x, node.translation.y - cube.size.y, node.translation.z - cube.size.z);
            writeVertex(
              node.translation.x + cube.size.x, node.translation.y - cube.size.y, node.translation.z - cube.size.z);
            writeVertex(
              node.translation.x + cube.size.x, node.translation.y + cube.size.y, node.translation.z - cube.size.z);
            writeVertex(
              node.translation.x - cube.size.x, node.translation.y + cube.size.y, node.translation.z - cube.size.z);
            writeVertex(
              node.translation.x - cube.size.x, node.translation.y - cube.size.y, node.translation.z + cube.size.z);
            writeVertex(
              node.translation.x + cube.size.x, node.translation.y - cube.size.y, node.translation.z + cube.size.z);
            writeVertex(
              node.translation.x + cube.size.x, node.translation.y + cube.size.y, node.translation.z + cube.size.z);
            writeVertex(
              node.translation.x - cube.size.x, node.translation.y + cube.size.y, node.translation.z + cube.size.z);

            // -Z
            writeFace(vertexIndex, vertexIndex + 2, vertexIndex + 1);
            writeFace(vertexIndex, vertexIndex + 3, vertexIndex + 2);
            // +Z
            writeFace(vertexIndex + 4, vertexIndex + 5, vertexIndex + 6);
            writeFace(vertexIndex + 4, vertexIndex + 6, vertexIndex + 7);
            // -X
            writeFace(vertexIndex, vertexIndex + 4, vertexIndex + 3);
            writeFace(vertexIndex + 3, vertexIndex + 4, vertexIndex + 7);
            // +X
            writeFace(vertexIndex + 1, vertexIndex + 2, vertexIndex + 6);
            writeFace(vertexIndex + 1, vertexIndex + 6, vertexIndex + 5);
            // -Y
            writeFace(vertexIndex, vertexIndex + 1, vertexIndex + 5);
            writeFace(vertexIndex, vertexIndex + 5, vertexIndex + 4);
            // +Y
            writeFace(vertexIndex + 2, vertexIndex + 3, vertexIndex + 7);
            writeFace(vertexIndex + 2, vertexIndex + 7, vertexIndex + 6);

            vertexIndex += 8;
        }
        else if (!node.mesh.isNone())
        {
            Mesh mesh;
            mesh.id = node.mesh.getId();
            getMesh(mesh);

            // Write vertices.
            for (const auto& [x, y, z] : mesh.vertices.get()) writeVertex(x, y, z);
            // Write triangle indices.
            for (const auto& [x, y, z] : mesh.indices.get())
                writeFace(vertexIndex + x, vertexIndex + y, vertexIndex + z);

            vertexIndex += static_cast<int32_t>(mesh.vertices.get().size());
        }
        else if (!node.sphere.isNone())
        {
            Sphere sphere;
            sphere.id = node.sphere.getId();
            getSphere(sphere);

            // Write +Z tip vertex.
            writeVertex(0, 0, sphere.radius);

            // Write ring vertices.
            constexpr int32_t rings       = 6;
            constexpr int32_t segments    = 8;
            constexpr int32_t vertexCount = 2 + (rings - 1) * segments;
            constexpr float   dtheta      = std::numbers::pi_v<float> / rings;
            constexpr float   dphi        = 2.0f * std::numbers::pi_v<float> / segments;
            float             theta       = 0;
            for (int32_t j = 0; j < rings - 1; j++)
            {
                theta += dtheta;
                float phi = 0.0f;
                for (int32_t i = 0; i < segments; i++)
                {
                    const float x = std::sinf(theta) * std::cosf(phi);
                    const float y = std::sinf(theta) * std::sinf(phi);
                    const float z = std::cosf(theta);
                    writeVertex(x * sphere.radius, y * sphere.radius, z * sphere.radius);
                    phi += dphi;
                }
            }

            // Write -Z tip vertex.
            writeVertex(0, 0, -sphere.radius);

            // Write bottom cap.
            for (int32_t i = 0; i < segments - 1; i++)
            {
                writeFace(vertexIndex, vertexIndex + 1 + i, vertexIndex + 2 + i);
            }
            writeFace(vertexIndex, vertexIndex + segments, vertexIndex + 1);

            // Write top cap.
            for (int32_t i = 0; i < segments - 1; i++)
            {
                writeFace(
                  vertexIndex + vertexCount - 1, vertexIndex + vertexCount - 2 - i, vertexIndex + vertexCount - 3 - i);
            }
            writeFace(
              vertexIndex + vertexCount - 1, vertexIndex + vertexCount - 1 - segments, vertexIndex + vertexCount - 2);

            // Write rings.
            for (int32_t j = 0; j < rings - 2; j++)
            {
                for (int32_t i = 0; i < segments - 1; i++)
                {
                    writeFace(vertexIndex + i + 1 + j * segments,
                              vertexIndex + i + 1 + (j + 1) * segments + 1,
                              vertexIndex + i + 1 + j * segments + 1);
                    writeFace(vertexIndex + i + 1 + j * segments,
                              vertexIndex + i + 1 + (j + 1) * segments,
                              vertexIndex + i + 1 + (j + 1) * segments + 1);
                }
                writeFace(vertexIndex + segments + j * segments,
                          vertexIndex + 1 + (j + 1) * segments,
                          vertexIndex + 1 + j * segments);
                writeFace(vertexIndex + segments + j * segments,
                          vertexIndex + (j + 2) * segments,
                          vertexIndex + 1 + (j + 1) * segments);
            }

            vertexIndex += vertexCount;
        }
    }
}