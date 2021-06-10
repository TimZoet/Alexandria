#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <filesystem>
#include <functional>

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "geometry/types.h"

bool importModel(const std::filesystem::path&                          path,
                 const std::function<void(std::shared_ptr<Mesh>)>&     insertMeshFunc,
                 const std::function<void(std::shared_ptr<Material>)>& insertMaterialFunc);