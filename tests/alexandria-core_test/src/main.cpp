#include <iostream>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "bettertest/run.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "alexandria-core_test/library/create_library.h"
#include "alexandria-core_test/member_types/member_type_blob.h"
#include "alexandria-core_test/member_types/member_type_blob_custom.h"
#include "alexandria-core_test/member_types/member_type_blob_array.h"
#include "alexandria-core_test/member_types/member_type_blob_array_custom.h"
//#include "alexandria-core_test/member_types/member_type_instance_id.h"
#include "alexandria-core_test/member_types/member_type_primitive_array.h"
#include "alexandria-core_test/member_types/member_type_primitive_array_custom.h"
#include "alexandria-core_test/member_types/member_type_primitive_blob.h"
#include "alexandria-core_test/member_types/member_type_primitive_blob_custom.h"
//#include "alexandria-core_test/member_types/member_type_reference.h"
//#include "alexandria-core_test/member_types/member_type_reference_array.h"
#include "alexandria-core_test/member_types/member_type_string_array.h"
#include "alexandria-core_test/member_types/member_type_string_array_custom.h"
#include "alexandria-core_test/types/create_type.h"
#include "alexandria-core_test/types/create_type_blob.h"
#include "alexandria-core_test/types/create_type_blob_array.h"
#include "alexandria-core_test/types/create_type_nested.h"
#include "alexandria-core_test/types/create_type_primitive.h"
#include "alexandria-core_test/types/create_type_primitive_array.h"
#include "alexandria-core_test/types/create_type_primitive_blob.h"
#include "alexandria-core_test/types/create_type_reference.h"
#include "alexandria-core_test/types/create_type_reference_array.h"
#include "alexandria-core_test/types/create_type_string.h"
#include "alexandria-core_test/types/create_type_string_array.h"

#ifdef WIN32
#include "Windows.h"
#endif

int main(const int argc, char** argv)
{
    // Set path next to executable.
#ifdef WIN32
    {
        WCHAR path[MAX_PATH];
        GetModuleFileNameW(nullptr, path, MAX_PATH);
        const std::filesystem::path workdir(path);
        const auto                  p = workdir.parent_path();
        current_path(p);
    }
#endif

    bt::run<
      // library
      CreateLibrary,
      // member_types
      MemberTypeBlob,
      MemberTypeBlobCustom,
      MemberTypeBlobArray,
      MemberTypeBlobArrayCustom,
      //MemberTypeInstanceId,
      MemberTypePrimitiveArray,
      MemberTypePrimitiveArrayCustom,
      MemberTypePrimitiveBlob,
      MemberTypePrimitiveBlobCustom,
      //MemberTypeReference,
      //MemberTypeReferenceArray,
      MemberTypeStringArray,
      MemberTypeStringArrayCustom,
      // types
      CreateType,
      CreateTypeBlob,
      CreateTypeBlobArray,
      CreateTypeNested,
      CreateTypePrimitive,
      CreateTypePrimitiveArray,
      CreateTypePrimitiveBlob,
      CreateTypeReference,
      CreateTypeReferenceArray,
      CreateTypeString,
      CreateTypeStringArray>(argc, argv, "alexandria-core");
    return 0;
}
