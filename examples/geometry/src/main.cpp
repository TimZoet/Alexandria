////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <iostream>
#include <string>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria/library.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "geometry/actions.h"

#ifdef WIN32
#include "geometry/winutils.h"
#endif

int main(int, char**)
{
    // Set path next to executable.
#ifdef WIN32
    setWinWorkDir();
#endif

    alex::LibraryPtr library;
    MaterialHandler  materialHandler;
    MeshHandler      meshHandler;

    std::string line;
    while (true)
    {
        // Get line from console.
        std::getline(std::cin, line);

        // Skip empty or exit.
        if (line.empty()) continue;
        if (line == "exit") return 0;

        // Run appropriate command.
        if (line.starts_with("cache "))
            actionCache(line.substr(6), materialHandler, meshHandler);
        else if (line == "clear")
            actionClear(materialHandler, meshHandler);
        else if (line.starts_with("export "))
            actionExport(line.substr(7), materialHandler, meshHandler);
        else if (line.starts_with("import "))
            actionImport(line.substr(7), materialHandler, meshHandler);
        else if (line.starts_with("link "))
            actionLink(line.substr(5), materialHandler, meshHandler);
        else if (line == "list")
            actionList(materialHandler, meshHandler);
        else if (line.starts_with("open "))
            actionOpen(line.substr(5), library, materialHandler, meshHandler);
        else if (line == "release")
            actionRelease(materialHandler, meshHandler);
        else if (line.starts_with("subdivide "))
            actionSubdivide(line.substr(10), meshHandler);
        else
            std::cout << "Unknown command" << std::endl;
    }
}
