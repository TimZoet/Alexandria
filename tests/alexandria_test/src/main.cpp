#include <iostream>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "bettertest/run.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "alexandria_test/delete/delete_blob.h"
#include "alexandria_test/delete/delete_blob_array.h"
#include "alexandria_test/delete/delete_invalid.h"
#include "alexandria_test/delete/delete_primitive.h"
#include "alexandria_test/delete/delete_primitive_array.h"
#include "alexandria_test/delete/delete_primitive_blob.h"
#include "alexandria_test/delete/delete_reference.h"
#include "alexandria_test/delete/delete_reference_array.h"
#include "alexandria_test/delete/delete_string.h"
#include "alexandria_test/delete/delete_string_array.h"
#include "alexandria_test/get/get_blob.h"
#include "alexandria_test/get/get_blob_array.h"
#include "alexandria_test/get/get_invalid.h"
#include "alexandria_test/get/get_primitive.h"
#include "alexandria_test/get/get_primitive_array.h"
#include "alexandria_test/get/get_primitive_blob.h"
#include "alexandria_test/get/get_reference.h"
#include "alexandria_test/get/get_reference_array.h"
#include "alexandria_test/get/get_string.h"
#include "alexandria_test/get/get_string_array.h"
#include "alexandria_test/insert/insert_blob.h"
#include "alexandria_test/insert/insert_blob_array.h"
#include "alexandria_test/insert/insert_invalid.h"
#include "alexandria_test/insert/insert_primitive.h"
#include "alexandria_test/insert/insert_primitive_array.h"
#include "alexandria_test/insert/insert_primitive_blob.h"
#include "alexandria_test/insert/insert_reference.h"
#include "alexandria_test/insert/insert_reference_array.h"
#include "alexandria_test/insert/insert_string.h"
#include "alexandria_test/insert/insert_string_array.h"
#include "alexandria_test/library/create_library.h"
#include "alexandria_test/member_types/member_type_blob.h"
#include "alexandria_test/member_types/member_type_blob_custom.h"
#include "alexandria_test/member_types/member_type_blob_array.h"
#include "alexandria_test/member_types/member_type_blob_array_custom.h"
//#include "alexandria_test/member_types/member_type_instance_id.h"
#include "alexandria_test/member_types/member_type_primitive_array.h"
#include "alexandria_test/member_types/member_type_primitive_array_custom.h"
#include "alexandria_test/member_types/member_type_primitive_blob.h"
#include "alexandria_test/member_types/member_type_primitive_blob_custom.h"
//#include "alexandria_test/member_types/member_type_reference.h"
//#include "alexandria_test/member_types/member_type_reference_array.h"
#include "alexandria_test/member_types/member_type_string_array.h"
#include "alexandria_test/member_types/member_type_string_array_custom.h"
#include "alexandria_test/table_sets/table_sets_blob.h"
#include "alexandria_test/table_sets/table_sets_blob_array.h"
#include "alexandria_test/table_sets/table_sets_nested.h"
#include "alexandria_test/table_sets/table_sets_primitive.h"
#include "alexandria_test/table_sets/table_sets_primitive_array.h"
#include "alexandria_test/table_sets/table_sets_primitive_blob.h"
#include "alexandria_test/table_sets/table_sets_query.h"
#include "alexandria_test/table_sets/table_sets_reference.h"
#include "alexandria_test/table_sets/table_sets_reference_array.h"
#include "alexandria_test/table_sets/table_sets_string.h"
#include "alexandria_test/table_sets/table_sets_string_array.h"
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
#include "alexandria_test/update/update_blob.h"
#include "alexandria_test/update/update_blob_array.h"
#include "alexandria_test/update/update_invalid.h"
#include "alexandria_test/update/update_primitive.h"
#include "alexandria_test/update/update_primitive_array.h"
#include "alexandria_test/update/update_primitive_blob.h"
#include "alexandria_test/update/update_reference.h"
#include "alexandria_test/update/update_reference_array.h"
#include "alexandria_test/update/update_string.h"
#include "alexandria_test/update/update_string_array.h"

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
      // delete
      DeleteBlob,
      DeleteBlobArray,
      DeleteInvalid,
      DeletePrimitive,
      DeletePrimitiveArray,
      DeletePrimitiveBlob,
      DeleteReference,
      DeleteReferenceArray,
      DeleteString,
      DeleteStringArray,
      // get
      GetBlob,
      GetBlobArray,
      GetInvalid,
      GetPrimitive,
      GetPrimitiveArray,
      GetPrimitiveBlob,
      GetReference,
      GetReferenceArray,
      GetString,
      GetStringArray,
      // insert
      InsertBlob,
      InsertBlobArray,
      InsertInvalid,
      InsertPrimitive,
      InsertPrimitiveArray,
      InsertPrimitiveBlob,
      InsertReference,
      InsertReferenceArray,
      InsertString,
      InsertStringArray,
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
      // table sets
      TableSetsBlob,
      TableSetsBlobArray,
      TableSetsNested,
      TableSetsPrimitive,
      TableSetsPrimitiveArray,
      TableSetsPrimitiveBlob,
      TableSetsQuery,
      TableSetsReference,
      TableSetsReferenceArray,
      TableSetsString,
      TableSetsStringArray,
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
      CreateTypeStringArray,
      // update
      UpdateBlob,
      UpdateBlobArray,
      UpdateInvalid,
      UpdatePrimitive,
      UpdatePrimitiveArray,
      UpdatePrimitiveBlob,
      UpdateReference,
      UpdateReferenceArray,
      UpdateString,
      UpdateStringArray>(argc, argv, "alexandria");
    return 0;
}
