#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <string>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria/library.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "geometry/types.h"

void actionCache(const std::string& param, MaterialHandler& materialHandler, MeshHandler& meshHandler);

void actionClear(MaterialHandler& materialHandler, MeshHandler& meshHandler);

void actionExport(const std::string& name, MaterialHandler& materialHandler, MeshHandler& meshHandler);

void actionImport(const std::string& file, MaterialHandler& materialHandler, MeshHandler& meshHandler);

void actionLink(const std::string& param, MaterialHandler& materialHandler, MeshHandler& meshHandler);

void actionList(MaterialHandler& materialHandler, MeshHandler& meshHandler);

void actionOpen(const std::string& file,
                alex::LibraryPtr&  library,
                MaterialHandler&   materialHandler,
                MeshHandler&       meshHandler);

void actionRelease(MaterialHandler& materialHandler, MeshHandler& meshHandler);

void actionSubdivide(const std::string& param, MeshHandler& meshHandler);