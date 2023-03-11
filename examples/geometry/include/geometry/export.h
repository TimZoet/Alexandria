#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <filesystem>

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "geometry/types/cube.h"
#include "geometry/types/material.h"
#include "geometry/types/mesh.h"
#include "geometry/types/scene.h"
#include "geometry/types/sphere.h"

void exportObj(const Scene&           scene,
               std::filesystem::path  path,
               Cube::get_query_t&     getCube,
               Material::get_query_t& getMaterial,
               Mesh::get_query_t&     getMesh,
               Sphere::get_query_t&   getSphere);
