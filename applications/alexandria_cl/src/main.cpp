////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <filesystem>
#include <iostream>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "parsertongue/parser.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "alexandria_cl/specification.h"

int main(int argc, char** argv)
{
    pt::parser parser(argc, argv);

    // Add arguments.
    auto library = parser.add_value<std::filesystem::path>('\0', "library");
    library->set_help("Path to library file");
    auto output = parser.add_value<std::filesystem::path>('o', "output");
    output->set_help("Path to output file");
    auto spec = parser.add_flag('\0', "specification");
    spec->set_help("Write specification graph to output file");

    // Run the parser.
    std::string e;
    if (!parser(e))
    {
        std::cout << "Internal parsing error: " << e << std::endl;
        return 0;
    }

    // User requested help or version, don't run.
    if (parser.display_help(std::cout)) return 0;

    if (spec->is_set())
    {
        if (!library->is_set())
        {
            std::cout << "Missing library argument" << std::endl;
            return 0;
        }

        if (!output->is_set())
        {
            std::cout << "Missing output argument" << std::endl;
            return 0;
        }

        writeSpecification(library->get_value(), output->get_value());
        return 0;
    }
}
