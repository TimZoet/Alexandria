#include <iostream>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "bettertest/run.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "alexandria-extended-query_test/search_queries/primitive_search.h"
#include "alexandria-extended-query_test/table_sets/table_sets_blob.h"
#include "alexandria-extended-query_test/table_sets/table_sets_blob_array.h"
#include "alexandria-extended-query_test/table_sets/table_sets_nested.h"
#include "alexandria-extended-query_test/table_sets/table_sets_primitive.h"
#include "alexandria-extended-query_test/table_sets/table_sets_primitive_array.h"
#include "alexandria-extended-query_test/table_sets/table_sets_primitive_blob.h"
#include "alexandria-extended-query_test/table_sets/table_sets_query.h"
#include "alexandria-extended-query_test/table_sets/table_sets_reference.h"
#include "alexandria-extended-query_test/table_sets/table_sets_reference_array.h"
#include "alexandria-extended-query_test/table_sets/table_sets_string.h"
#include "alexandria-extended-query_test/table_sets/table_sets_string_array.h"

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
      // search queries
      PrimitiveSearch,
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
      TableSetsStringArray>(argc, argv, "alexandria-extended-query");
    return 0;
}
