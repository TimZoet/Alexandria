#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <algorithm>

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
    namespace detail
    {
        template<auto...>
        struct get_last_mp
        {
        };

        template<auto M>
        struct get_last_mp<M>
        {
            static constexpr auto value = M;
        };

        template<auto M, auto... Ms>
        struct get_last_mp<M, Ms...> : get_last_mp<Ms...>
        {
        };

        template<auto...>
        struct is_mp_sequence_impl : std::false_type
        {
        };

        template<auto M>
        struct is_mp_sequence_impl<M> : std::true_type
        {
        };

        template<auto M, auto N, auto... Ms>
        struct is_mp_sequence_impl<M, N, Ms...> : std::false_type
        {
        };

        template<auto M, auto N, auto... Ms>
            requires(std::same_as<member_pointer_value_t<std::remove_cvref_t<decltype(M)>>,
                                  member_pointer_class_t<std::remove_cvref_t<decltype(N)>>>)
        struct is_mp_sequence_impl<M, N, Ms...> : is_mp_sequence_impl<N, Ms...>
        {
        };

        template<auto... Ms>
        concept is_mp_sequence = is_mp_sequence_impl<Ms...>::value;

        template<size_t N>
        struct MemberName
        {
            static constexpr size_t size = N;

            template<size_t A, size_t B>
            constexpr MemberName(const char (&strA)[A], const char (&strB)[B])
            {
                // Copy without null terminator.
                std::copy_n(strA, A - 1, name);
                // Place '.' inbetween.
                *(name + A - 1) = '.';
                // Copy with null terminator.
                std::copy_n(strB, B, name + A);
            }

            constexpr MemberName(const char (&str)[N]) { std::copy_n(str, N, name); }

            char name[N] = {};
        };

        /**
         * \brief Concatenate two  MemberName types. Strings are concatenated as "A.B".
         * \tparam A Left.
         * \tparam B Right.
         * \return MemberName.
         */
        template<MemberName A, MemberName B>
        constexpr auto concat()
        {
            return MemberName<decltype(A)::size + decltype(B)::size>(A.name, B.name);
        }

        /**
         * \brief Perform an exact comparison of two MemberName types.
         * \tparam A Left.
         * \tparam B Right.
         * \return Equality.
         */
        template<MemberName A, MemberName B>
        constexpr bool compare()
        {
            // If A and B have same lengths, compare strings.
            if constexpr (std::same_as<decltype(A), decltype(B)>)
            {
                constexpr auto cmp = [](const char* a, const char* b) -> bool {
                    while (*a || *b)
                        if (*a++ != *b++) return false;
                    return true;
                };

                return cmp(A.name, B.name);
            }

            return false;
        }
    }  // namespace detail

    ////////////////////////////////////////////////////////////////
    // Member.
    ////////////////////////////////////////////////////////////////

    template<detail::MemberName Name, auto M, auto... Ms>
        requires(detail::is_mp_sequence<M, Ms...>)
    class Member
    {
    public:
        inline static constexpr bool is_member_v = true;
        inline static constexpr auto mp          = detail::get_last_mp<M, Ms...>::value;
        using class_t                            = member_pointer_class_t<std::decay_t<decltype(M)>>;
        using value_t                            = member_pointer_value_t<std::decay_t<decltype(mp)>>;
        inline static constexpr auto name_v      = Name;

        static constexpr bool is_instance_id     = is_instance_id_mp<mp>;
        static constexpr bool is_blob            = is_blob_mp<mp>;
        static constexpr bool is_blob_array      = is_blob_array_mp<mp>;
        static constexpr bool is_primitive       = is_primitive_mp<mp>;
        static constexpr bool is_primitive_array = is_primitive_array_mp<mp>;
        static constexpr bool is_primitive_blob  = is_primitive_blob_mp<mp>;
        static constexpr bool is_reference       = is_reference_mp<mp>;
        static constexpr bool is_reference_array = is_reference_array_mp<mp>;
        static constexpr bool is_string          = is_string_mp<mp>;
        static constexpr bool is_string_array    = is_string_array_mp<mp>;

        static value_t& get(class_t& obj) { return get<class_t, M, Ms...>(obj); }

        static const value_t& get(const class_t& obj) { return getConst<class_t, M, Ms...>(obj); }

    private:
        template<typename U, auto P, auto... Ps>
        static auto& get(U& obj)
        {
            if constexpr (sizeof...(Ps) > 0)
                return get<decltype(obj.*P), Ps...>(obj.*P);
            else
                return obj.*P;
        }

        template<typename U, auto P, auto... Ps>
        static const auto& getConst(const U& obj)
        {
            if constexpr (sizeof...(Ps) > 0)
                return get<decltype(obj.*P), Ps...>(obj.*P);
            else
                return obj.*P;
        }
    };

    ////////////////////////////////////////////////////////////////
    // Type traits.
    ////////////////////////////////////////////////////////////////

    template<typename T>
    concept is_member = requires { T::is_member_v; };

    template<typename...>
    struct IsMemberListImpl : std::false_type
    {
    };

    template<typename T>
    concept is_member_list = IsMemberListImpl<T>::value;

    template<typename...>
    struct IsNestedMemberImpl : std::false_type
    {
    };

    template<typename T>
    concept is_nested_member = IsNestedMemberImpl<T>::value;

    ////////////////////////////////////////////////////////////////
    // MemberList.
    ////////////////////////////////////////////////////////////////

    template<typename T, typename... Ts>
    concept is_same_class_member = ((is_member<T> || is_nested_member<T>) && ... &&
                                    (is_member<Ts> || is_nested_member<Ts>)) &&
                                   (std::same_as<typename T::class_t, typename Ts::class_t> && ...);

    template<typename T, typename... Ts>
        requires(is_same_class_member<T, Ts...>)
    class MemberList
    {
    public:
        using class_t = typename T::class_t;
    };

    template<typename T, typename... Ts>
    struct IsMemberListImpl<MemberList<T, Ts...>> : std::true_type
    {
    };

    ////////////////////////////////////////////////////////////////
    // NestedMember.
    ////////////////////////////////////////////////////////////////

    template<detail::MemberName Name, typename T, auto M>
        requires(is_member_list<T> && std::same_as<typename T::class_t, member_pointer_value_t<decltype(M)>>)
    class NestedMember
    {
    public:
        using class_t = member_pointer_class_t<decltype(M)>;
    };

    template<detail::MemberName Name, typename T, auto M>
    struct IsNestedMemberImpl<NestedMember<Name, T, M>> : std::true_type
    {
    };

    ////////////////////////////////////////////////////////////////
    // MemberChain.
    ////////////////////////////////////////////////////////////////

    template<typename M, typename... Ts>
    class MemberChain
    {
    public:
        inline static constexpr bool is_member_chain_v = true;
        inline static constexpr auto size              = sizeof...(Ts);
    };

    template<typename T>
    concept is_member_chain = requires { T::is_member_chain_v; };
}  // namespace alex