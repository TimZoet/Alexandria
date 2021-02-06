#include <iostream>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "bettertest/run.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "alexandria_test/create_library.h"
#include "alexandria_test/create_type_blob.h"
#include "alexandria_test/create_type_blob_array.h"
#include "alexandria_test/create_type_nested.h"

#ifdef WIN32
#include "Windows.h"
#endif

int main(int argc, char** argv)
{
    // Set path next to executable.
#ifdef WIN32
    {
        WCHAR path[MAX_PATH];
        GetModuleFileNameW(nullptr, path, MAX_PATH);
        const std::filesystem::path workdir(path);
        const auto                  p = workdir.parent_path();
        std::filesystem::current_path(p);
    }
#endif

    bt::run<CreateLibrary, CreateTypeBlob, CreateTypeBlobArray, CreateTypeNested>(argc, argv, "alexandria");
    return 0;
}
