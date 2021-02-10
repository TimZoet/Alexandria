#include <iostream>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "bettertest/run.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "alexandria_test/create_library.h"
#include "alexandria_test/create_property.h"
#include "alexandria_test/create_property_blob.h"
#include "alexandria_test/create_property_nested.h"
#include "alexandria_test/create_property_primitive.h"
#include "alexandria_test/create_type.h"
#include "alexandria_test/create_type_blob.h"
#include "alexandria_test/create_type_blob_array.h"
#include "alexandria_test/create_type_nested.h"
#include "alexandria_test/create_type_nested_array.h"
#include "alexandria_test/create_type_nested_reference.h"
#include "alexandria_test/create_type_primitive.h"
#include "alexandria_test/create_type_primitive_array.h"
#include "alexandria_test/create_type_primitive_blob.h"
#include "alexandria_test/type_add_property.h"

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

    bt::run<CreateLibrary,
            CreateProperty,
            CreatePropertyBlob,
            CreatePropertyNested,
            CreatePropertyPrimitive,
            CreateType,
            CreateTypeBlob,
            CreateTypeBlobArray,
            CreateTypeNested,
            CreateTypeNestedArray,
            CreateTypeNestedReference,
            CreateTypePrimitive,
            CreateTypePrimitiveArray,
            CreateTypePrimitiveBlob,
            TypeAddProperty>(argc, argv, "alexandria");
    return 0;
}
