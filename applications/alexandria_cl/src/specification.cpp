#include "alexandria_cl/specification.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <fstream>
#include <iostream>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria/library.h"

void writeSpecification(const std::filesystem::path& library, const std::filesystem::path& output)
{
    // Open library.
    alex::LibraryPtr lib;
    try
    {
        lib = alex::Library::open(library);
    }
    catch (const std::runtime_error& e)
    {
        std::cout << e.what() << std::endl;
        return;
    }

    // Open output file.
    std::ofstream file(output);
    if (!file)
    {
        std::cout << "Could not open " << output << std::endl;
        return;
    }

    // Write graph.
    lib->writeGraph(file);
}