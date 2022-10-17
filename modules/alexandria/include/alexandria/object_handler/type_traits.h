#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <type_traits>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "common/type_traits.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "alexandria/member_types/blob.h"
#include "alexandria/member_types/blob_array.h"
#include "alexandria/member_types/primitive.h"
#include "alexandria/member_types/primitive_array.h"
#include "alexandria/member_types/primitive_blob.h"
#include "alexandria/member_types/reference.h"
#include "alexandria/member_types/reference_array.h"
#include "alexandria/member_types/string.h"
#include "alexandria/member_types/string_array.h"

namespace alex
{
    ////////////////////////////////////////////////////////////////
    // type_to_column.
    ////////////////////////////////////////////////////////////////

    template<typename T>
    struct type_to_column
    {
    };

    template<is_primitive T>
    struct type_to_column<T>
    {
        using type = T;
    };

    template<is_string T>
    struct type_to_column<T>
    {
        using type = std::string;
    };

    template<is_reference T>
    struct type_to_column<T>
    {
        using type = sql::row_id;
    };

    template<is_primitive_array T>
    struct type_to_column<T>
    {
        using type = typename T::element_t;
    };

    template<is_string_array T>
    struct type_to_column<T>
    {
        using type = typename T::element_t;
    };

    template<is_primitive_blob T>
    struct type_to_column<T>
    {
        using type = typename T::blob_t;
    };

    template<is_blob T>
    struct type_to_column<T>
    {
        using type = typename T::blob_t;
    };

    template<is_blob_array T>
    struct type_to_column<T>
    {
        using type = typename T::blob_t;
    };

    template<is_reference_array T>
    struct type_to_column<T>
    {
        using type = sql::row_id;
    };

    /**
     * \brief Convert a type to a column type that can be passed to cppql.
     * \tparam T Type.
     */
    template<typename T>
    using type_to_column_t = typename type_to_column<T>::type;
}  // namespace alex