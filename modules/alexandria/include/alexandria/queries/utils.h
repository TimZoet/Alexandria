#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <tuple>
#include <type_traits>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "common/type_traits.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "alexandria/core/member.h"
#include "alexandria/member_types/instance_id.h"
#include "alexandria/member_types/reference.h"

namespace alex::detail
{
    ////////////////////////////////////////////////////////////////
    // ...
    ////////////////////////////////////////////////////////////////

    template<typename T>
    struct ValueToColumnType
    {
        using type = T;
    };

    template<is_instance_id T>
    struct ValueToColumnType<T>
    {
        using type = std::string;
    };

    template<is_primitive_array T>
    struct ValueToColumnType<T>
    {
        using type = typename T::element_t;
    };

    template<is_primitive_blob T>
    struct ValueToColumnType<T>
    {
        using type = typename T::value_t;
    };

    template<is_string_array T>
    struct ValueToColumnType<T>
    {
        using type = std::string;
    };

    template<is_blob T>
    struct ValueToColumnType<T>
    {
        using type = typename T::value_t;
    };

    template<is_blob_array T>
    struct ValueToColumnType<T>
    {
        using type = typename T::element_t;
    };

    template<is_reference T>
    struct ValueToColumnType<T>
    {
        using type = std::string;
    };


    /**
     * \brief 
     * \tparam M 
     */
    template<is_member M>
    using member_to_column_t = typename ValueToColumnType<typename M::value_t>::type;

    template<typename...>
    struct MembersToTypes
    {
    };

    template<is_member... Ms>
    struct MembersToTypes<std::tuple<Ms...>>
    {
        using type = std::tuple<sql::row_id, member_to_column_t<Ms>...>;
    };

    template<typename T>
    using member_tuple_to_column_t = typename MembersToTypes<T>::type;

    /**
     * \brief Generate a TypedTable for a std::tuple<Member<primitive*>...>.
     * Resulting table will have sizeof...(Member<primitive*>...) columns.
     * \tparam T Tuple of primitive members.
     */
    template<typename T>
    using primitive_table_t = sql::tuple_to_table_t<member_tuple_to_column_t<T>>;

    /**
     * \brief Generate a TypedTable for a Member<PrimitiveArray*>. Resulting table will have 3 columns:
     * sql::rowid (row index)
     * std::string (foreign key to InstanceTable)
     * primitive  (element type held by array)
     * \tparam M PrimitiveArray member type.
     */
    template<is_member M>
    using primitive_array_table_t = sql::TypedTable<sql::row_id, std::string, member_to_column_t<M>>;

    /**
     * \brief Generate a TypedTable for a Member<ReferenceArray*>. Resulting table will have 3 columns:
     * sql::rowid (row index)
     * std::string (foreign key to InstanceTable)
     * std::string (foreign key to referenced InstanceTable)
     * \tparam M PrimitiveArray member type.
     */
    template<is_member M>
    using reference_array_table_t = sql::TypedTable<sql::row_id, std::string, std::string>;

    /**
     * \brief Generate a TypedTable for a Member<BlobArray*>. Resulting table will have 3 columns:
     * sql::rowid (row index)
     * std::string (foreign key to InstanceTable)
     * blob (element type held by array)
     * \tparam M PrimitiveArray member type.
     */
    template<is_member M>
    using blob_array_table_t = sql::TypedTable<sql::row_id, std::string, member_to_column_t<M>>;
}  // namespace alex::detail
