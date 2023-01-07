#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <tuple>
#include <type_traits>

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "alexandria/core/member.h"
#include "alexandria/member_types/blob_array.h"
#include "alexandria/member_types/instance_id.h"
#include "alexandria/member_types/reference_array.h"

namespace alex::detail
{
    /**
     * \brief 
     * \tparam I Index used for terminating to prevent wrap-around.
     * \tparam E Extractor type that filters out elements from Members.
     * \tparam Members List of Member types.
     */
    template<size_t I, template<typename> class E, typename... Members>
    struct MemberExtractorImpl
    {
    };

    // No matching type, drop Member.
    template<size_t I, template<typename> class E, is_member Member, typename... Members>
    struct MemberExtractorImpl<I, E, Member, Members...> : MemberExtractorImpl<I - 1, E, Members...>
    {
    };

    // Matching type, append Member to end.
    template<size_t I, template<typename> class E, is_member Member, typename... Members>
        requires(E<Member>::value)
    struct MemberExtractorImpl<I, E, Member, Members...> : MemberExtractorImpl<I - 1, E, Members..., Member>
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
    template<template<typename> class E, typename Member, typename... Members>
    struct MemberExtractorImpl<0, E, Member, Members...>
    {
        using type = std::tuple<Member, Members...>;
    };

    template<template<typename> class E, typename...>
    struct MemberExtractor
    {
    };

    template<template<typename> class E, typename... Members>
    struct MemberExtractor<E, std::tuple<Members...>>
    {
        // Reroutes std::tuple types into a parameter pack.
        using type = typename MemberExtractorImpl<sizeof...(Members), E, Members...>::type;
    };

    ////////////////////////////////////////////////////////////////
    // Primitive member extraction (includes InstanceId, primitives, primitive blobs, strings, blobs, references).
    ////////////////////////////////////////////////////////////////

    template<typename...>
    struct PrimitiveExtractor : std::false_type
    {
    };

    template<is_member M>
        requires(M::is_instance_id || M::is_primitive || M::is_string || M::is_blob || M::is_reference ||
                 M::is_primitive_blob)
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

    template<is_member M>
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

    template<is_member M>
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

    template<is_member M>
        requires(M::is_reference_array)
    struct ReferenceArrayExtractor<M> : std::true_type
    {
    };

    /**
     * \brief Takes a tuple of members and returns a tuple with all non reference array members filtered out.
     * \tparam T Tuple of all members without UuidMember.
     */
    template<typename T>
    using extract_reference_array_members_t = typename MemberExtractor<ReferenceArrayExtractor, T>::type;
}  // namespace alex::detail
