#include "geometry/winutils.h"



#ifdef WIN32

#include <filesystem>
#include "Windows.h"

void setWinWorkDir()
{
    WCHAR path[MAX_PATH];
    GetModuleFileNameW(nullptr, path, MAX_PATH);
    const std::filesystem::path workdir(path);
    const auto                  p = workdir.parent_path();
    current_path(p);
}

#endif