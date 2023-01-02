#include <iostream>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "bettertest/run.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

//#include "alexandria_test/create/create_direct.h"
//#include "alexandria_test/create/create_managed.h"
//#include "alexandria_test/create/create_primitive.h"
//#include "alexandria_test/create/create_string.h"
//#include "alexandria_test/delete/delete_blob.h"
//#include "alexandria_test/delete/delete_blob_array.h"
//#include "alexandria_test/delete/delete_direct.h"
//#include "alexandria_test/delete/delete_managed.h"
//#include "alexandria_test/delete/delete_primitive.h"
//#include "alexandria_test/delete/delete_primitive_array.h"
//#include "alexandria_test/delete/delete_primitive_blob.h"
//#include "alexandria_test/delete/delete_reference.h"
//#include "alexandria_test/delete/delete_reference_array.h"
//#include "alexandria_test/delete/delete_string.h"
//#include "alexandria_test/delete/delete_string_array.h"
//#include "alexandria_test/get/get_blob.h"
//#include "alexandria_test/get/get_blob_array.h"
//#include "alexandria_test/get/get_direct.h"
//#include "alexandria_test/get/get_managed.h"
//#include "alexandria_test/get/get_primitive.h"
//#include "alexandria_test/get/get_primitive_array.h"
//#include "alexandria_test/get/get_primitive_blob.h"
//#include "alexandria_test/get/get_reference.h"
//#include "alexandria_test/get/get_reference_array.h"
//#include "alexandria_test/get/get_string.h"
//#include "alexandria_test/get/get_string_array.h"
//#include "alexandria_test/insert/insert_blob.h"
//#include "alexandria_test/insert/insert_blob_array.h"
//#include "alexandria_test/insert/insert_direct.h"
//#include "alexandria_test/insert/insert_managed.h"
#include "alexandria_test/insert/insert_primitive.h"
//#include "alexandria_test/insert/insert_primitive_array.h"
//#include "alexandria_test/insert/insert_primitive_blob.h"
//#include "alexandria_test/insert/insert_reference.h"
//#include "alexandria_test/insert/insert_reference_array.h"
//#include "alexandria_test/insert/insert_string.h"
//#include "alexandria_test/insert/insert_string_array.h"
#include "alexandria_test/library/create_library.h"
//#include "alexandria_test/member_types/member_type_blob.h"
//#include "alexandria_test/member_types/member_type_blob_array.h"
//#include "alexandria_test/member_types/member_type_instance_id.h"
//#include "alexandria_test/member_types/member_type_primitive_array.h"
//#include "alexandria_test/member_types/member_type_primitive_blob.h"
//#include "alexandria_test/member_types/member_type_reference.h"
//#include "alexandria_test/member_types/member_type_reference_array.h"
//#include "alexandria_test/member_types/member_type_string_array.h"
#include "alexandria_test/types/create_type.h"
#include "alexandria_test/types/create_type_blob.h"
#include "alexandria_test/types/create_type_blob_array.h"
#include "alexandria_test/types/create_type_nested.h"
#include "alexandria_test/types/create_type_primitive.h"
#include "alexandria_test/types/create_type_primitive_array.h"
#include "alexandria_test/types/create_type_primitive_blob.h"
#include "alexandria_test/types/create_type_reference.h"
#include "alexandria_test/types/create_type_reference_array.h"
#include "alexandria_test/types/create_type_string.h"
#include "alexandria_test/types/create_type_string_array.h"
//#include "alexandria_test/update/update_direct.h"
//#include "alexandria_test/update/update_managed.h"

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

    bt::run<
      // create
      //CreateDirect,
      //CreateManaged,
      //CreatePrimitive,
      //CreateString,
      // delete
      //DeleteBlob,
      //DeleteBlobArray,
      //DeleteDirect,
      //DeleteManaged,
      //DeletePrimitive,
      //DeletePrimitiveArray,
      //DeletePrimitiveBlob,
      //DeleteReference,
      //DeleteReferenceArray,
      //DeleteString,
      //DeleteStringArray,
      // get
      //GetBlob,
      //GetBlobArray,
      //GetDirect,
      //GetManaged,
      //GetPrimitive,
      //GetPrimitiveArray,
      //GetPrimitiveBlob,
      //GetReference,
      //GetReferenceArray,
      //GetString,
      //GetStringArray,
      // insert
      //InsertBlob,
      //InsertBlobArray,
      //InsertDirect,
      //InsertManaged,
      InsertPrimitive,
      //InsertPrimitiveArray,
      //InsertPrimitiveBlob,
      //InsertReference,
      //InsertReferenceArray,
      //InsertString,
      //InsertStringArray,
      // library
      CreateLibrary,
      // member_types
      //MemberTypeBlob,
      //MemberTypeBlobArray,
      //MemberTypeInstanceId,
      //MemberTypePrimitiveArray,
      //MemberTypePrimitiveBlob,
      //MemberTypeReference,
      //MemberTypeReferenceArray,
      //MemberTypeStringArray,
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
      CreateTypeStringArray
      // update
      //UpdateDirect,
      //UpdateManaged
    >(argc, argv, "alexandria");
    return 0;
}
