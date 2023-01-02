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
    template<size_t, template<typename> class E, typename...>
    struct MemberExtractorImpl
    {
    };

    // No matching type, drop M.
    template<size_t I, template<typename> class E, typename M, typename... Members>
    struct MemberExtractorImpl<I, E, M, Members...> : MemberExtractorImpl<I - 1, E, Members...>
    {
    };

    // Matching type, append M to end.
    template<size_t I, template<typename> class E, typename M, typename... Members>
        requires(E<M>::value)
    struct MemberExtractorImpl<I, E, M, Members...> : MemberExtractorImpl<I - 1, E, Members..., M>
    {
    };

    // Terminate when I == 0.
    template<template<typename> class E, typename... Members>
    struct MemberExtractorImpl<0, E, Members...>
    {
        using type = std::tuple<Members...>;
    };

    // Terminate when I == 0.
    // Note: The one termination definition above is not enough. When the original first member was a primitive type,
    // it ends up at the head again. This causes the compiler to pick the append definition again, with I now set to
    // -1 unsigned, causing an endless loop. Just this definition is also not enough, because then empty lists are not
    // handled.
    template<template<typename> class E, typename M, typename... Members>
    struct MemberExtractorImpl<0, E, M, Members...>
    {
        using type = std::tuple<M, Members...>;
    };

    template<template<typename> class E, typename...>
    struct MemberExtractor
    {
    };

    template<template<typename> class E, typename... Members>
    struct MemberExtractor<E, std::tuple<Members...>>
    {
        using type = typename MemberExtractorImpl<sizeof...(Members), E, Members...>::type;
    };

    ////////////////////////////////////////////////////////////////
    // Primitive member extraction (includes primitives, primitive blobs, strings, blobs, references).
    ////////////////////////////////////////////////////////////////

    template<typename...>
    struct PrimitiveExtractor : std::false_type
    {
    };

    template<typename M>
        requires(M::is_primitive || M::is_string || M::is_blob || M::is_reference || M::is_primitive_blob)
    struct PrimitiveExtractor<M> : std::true_type
    {
    };

    /**
     * \brief Takes a tuple of members and returns a tuple with all non primitive members filtered out.
     * \tparam T Tuple of all members without UuidMember.
     */
    template<typename T>
    using extract_primitive_members_t = typename MemberExtractor<PrimitiveExtractor, T>::type;

    ////////////////////////////////////////////////////////////////
    // Primitive array member extraction (includes primitives, strings).
    ////////////////////////////////////////////////////////////////

    template<typename...>
    struct PrimitiveArrayExtractor : std::false_type
    {
    };

    template<typename M>
        requires(M::is_primitive_array || M::is_string_array)
    struct PrimitiveArrayExtractor<M> : std::true_type
    {
    };

    /**
     * \brief Takes a tuple of members and returns a tuple with all non primitive array members filtered out.
     * \tparam T Tuple of all members without UuidMember.
     */
    template<typename T>
    using extract_primitive_array_members_t = typename MemberExtractor<PrimitiveArrayExtractor, T>::type;

    ////////////////////////////////////////////////////////////////
    // Blob array member extraction.
    ////////////////////////////////////////////////////////////////

    template<typename...>
    struct BlobArrayExtractor : std::false_type
    {
    };

    template<typename M>
        requires(M::is_blob_array)
    struct BlobArrayExtractor<M> : std::true_type
    {
    };

    /**
     * \brief Takes a tuple of members and returns a tuple with all non blob array members filtered out.
     * \tparam T Tuple of all members without UuidMember.
     */
    template<typename T>
    using extract_blob_array_members_t = typename MemberExtractor<BlobArrayExtractor, T>::type;

    ////////////////////////////////////////////////////////////////
    // Reference array member extraction.
    ////////////////////////////////////////////////////////////////

    template<typename...>
    struct ReferenceArrayExtractor : std::false_type
    {
    };

    template<typename M>
        requires(M::is_blob_array)
    struct ReferenceArrayExtractor<M> : std::true_type
    {
    };

    /**
     * \brief Takes a tuple of members and returns a tuple with all non reference array members filtered out.
     * \tparam T Tuple of all members without UuidMember.
     */
    template<typename T>
    using extract_reference_array_members_t = typename MemberExtractor<ReferenceArrayExtractor, T>::type;

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

    template<is_reference T>
    struct ValueToColumnType<T>
    {
        using type = std::string;
    };

    template<typename...>
    struct MembersToTypes
    {
    };

    template<typename... Ms>
    struct MembersToTypes<std::tuple<Ms...>>
    {
        using type = std::tuple<typename ValueToColumnType<typename Ms::value_t>::type...>;
    };
    /**
     * \brief 
     * \tparam T Tuple of primitive members.
     */
    template<typename T>
    using primitive_table_t = sql::tuple_to_table_t<typename MembersToTypes<T>::type>;

    //using primitive_array_table_t = ...;
    //using blob_array_table_t = ...;
    //using reference_array_table_t = sql::TypedTable<sql::row_id, std::string, std::string>;
}  // namespace alex::detail
