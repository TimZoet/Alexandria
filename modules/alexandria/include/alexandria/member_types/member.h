#pragma once

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

    template<auto M, auto... Ms>
    class Member
    {
    public:
        // TODO: Constrain M and Ms... to be sequence of member pointers.

        inline static constexpr bool is_member_v = true;
        inline static constexpr auto mp          = get_last_mp<M, Ms...>::value;
        using class_t                            = member_pointer_class_t<std::decay_t<decltype(M)>>;
        using value_t                            = member_pointer_value_t<std::decay_t<decltype(mp)>>;

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

    template<typename T, typename... Ts>  //requires(T::is_member_v || T::is_nested_member_v)
    class MemberList
    {
    public:
        inline static constexpr bool is_member_list_v = true;
    };

    template<typename T, auto M>  //requires(T::is_member_list_v)
    class NestedMember
    {
    public:
        inline static constexpr bool is_nested_member_v = true;
    };

    template<typename M, typename... Ts>
    class MemberChain
    {
    public:
        inline static constexpr bool is_member_chain_v = true;
        inline static constexpr auto size              = sizeof...(Ts);
    };

    template<typename T, auto... M0s>
    auto concat(Member<M0s...>)
    {
        auto f = []<auto... M1s>(Member<M1s...>) { return Member<M0s..., M1s...>(); };
        return f(T());
    }

    template<typename M0, typename M1>
    using concat_t = decltype(concat<M1>(std::declval<M0>()));

    template<typename T>
    concept is_member = requires
    {
        T::is_member_v;
    };

    template<typename T>
    concept is_member_list = requires
    {
        T::is_member_list_v;
    };

    template<typename T>
    concept is_nested_member = requires
    {
        T::is_nested_member_v;
    };

    template<typename T>
    concept is_member_chain = requires
    {
        T::is_member_chain_v;
    };

    class GenerateMemberTuple
    {
    public:
        // The get method takes a list of Member and NestedMember types. It turns this into a tuple of Member types
        // through step-by-step conversions of the first parameter and then recursing on the remaining ones.
        //
        // There are 6 rules that are used to do these conversions. The first 2 rules convert Member and NestedMember types into MemberChains:
        //
        // rule0 = Member<Ms*...>                                                              -> place Member<Ms*...> into resolved tuple
        // rule1 = NestedMember<MemberList<Ts...>, M*>                                         -> make MemberChain<Member<M*>, Ts...>
        //
        // The next 3 rules extract Members from a MemberChain or further expand NestedMembers.
        //
        // rule2 = MemberChain<Member<M0s*...>, Member<M1s*...>, Ts...>                        -> make Member<M0s*..., M1s*...>, MemberChain<Member<M0s*...>, Ts...>
        // rule3 = MemberChain<Member<M1s*...>, NestedMember<MemberList<T1s...>, M2*>, T2s...> -> make MemberChain<Member<M1s*..., M2*>, T1s...>..., MemberChain<Member<M1s*...>, T2s...>
        // rule4 = MemberChain<Member<Ms*...>>                                                 -> remove MemberChain<Member<Ms*...>>
        //
        // The final rule terminates and returns the resolved types.
        //
        // rule5 = <empty>                                                                     -> return resolved tuple

        template<typename T, typename... Ts>
        static auto get()
        {
            return getImpl<std::tuple<>, T, Ts...>();
        }

    private:
        // rule0
        template<typename Resolved, is_member T, typename... Ts>
        static auto getImpl()
        {
            return getImpl<tuple_cat_t<Resolved, T>, Ts...>();
        }

        // rule1
        template<typename Resolved, is_nested_member T, typename... Ts>
        static auto getImpl()
        {
            auto f = [&]<auto M, typename... Us>(NestedMember<MemberList<Us...>, M>)
            {
                return getImpl<Resolved, MemberChain<Member<M>, Us...>, Ts...>();
            };
            return f(T());
        }

        // rule2 && rule3 && rule4
        template<typename Resolved, is_member_chain T, typename... Ts>
        static auto getImpl()
        {
            // rule4
            if constexpr (T::size == 0) { return getImpl<Resolved, Ts...>(); }
            else
            {
                auto f = [&]<typename M, typename U, typename... Us>(MemberChain<M, U, Us...>)
                {
                    // rule2
                    if constexpr (is_member<U>)
                    {
                        return getImpl<Resolved, concat_t<M, U>, MemberChain<M, Us...>, Ts...>();
                    }
                    // rule3
                    else if constexpr (is_nested_member<U>)
                    {
                        auto g = [&]<auto NM, typename... Xs>(NestedMember<MemberList<Xs...>, NM>)
                        {
                            using c0 = MemberChain<concat_t<M, Member<NM>>, Xs...>;
                            using c1 = MemberChain<M, Us...>;
                            return getImpl<Resolved, c0, c1, Ts...>();
                        };
                        return g(U());
                    }
                };
                return f(T());
            }
        }

        // rule5
        template<typename Resolved>
        static auto getImpl()
        {
            return Resolved();
        }
    };
}  // namespace alex