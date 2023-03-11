#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <filesystem>

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "geometry/types/material.h"
#include "geometry/types/mesh.h"
#include "geometry/types/node.h"
#include "geometry/types/scene.h"

bool importObj(const std::filesystem::path& path,
               MaterialInsertQuery&         insertMaterial,
               MeshInsertQuery&             insertMesh,
               NodeInsertQuery&             insertNode,
               SceneInsertQuery&            insertScene);
