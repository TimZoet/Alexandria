#pragma once

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "alexandria/core/member.h"
#include "alexandria/core/type.h"

namespace alex
{
    // TODO: Verify that all members have a unique name.

    template<typename UuidMember, typename... Members>
    class TypeDescriptor
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        using object_t       = typename UuidMember::class_t;
        using uuid_member_t  = UuidMember;
        using members_t      = std::tuple<UuidMember, Members...>;
        using user_members_t = std::tuple<Members...>;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        TypeDescriptor() = default;

        explicit TypeDescriptor(Type& t) : type(&t)
        {
            // TODO: Do a runtime check to verify that Members match properties of type.
        }

        TypeDescriptor(const TypeDescriptor&) = default;

        TypeDescriptor(TypeDescriptor&&) noexcept = default;

        ~TypeDescriptor() noexcept = default;

        TypeDescriptor& operator=(const TypeDescriptor&) = default;

        TypeDescriptor& operator=(TypeDescriptor&&) noexcept = default;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] Type& getType() noexcept { return *type; }

        [[nodiscard]] const Type& getType() const noexcept { return *type; }

    private:

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        Type* type;
    };

    namespace detail
    {
        template<typename T, MemberName Name0, auto... M0s>
        auto concat(Member<Name0, M0s...>)
        {
            auto f = []<MemberName Name1, auto... M1s>(Member<Name1, M1s...>)
            {
                return Member<detail::concat<Name0, Name1>(), M0s..., M1s...>();
            };
            return f(T());
        }

        template<typename M0, typename M1>
        using concat_t = decltype(concat<M1>(std::declval<M0>()));

        class GenerateTypeDescriptorImpl
        {
        public:
            // The get method takes a list of Member and NestedMember types. It turns this into a tuple of Member types
            // through step-by-step conversions of the first parameter and then recursing on the remaining ones. Note
            // that concatenation of MemberNames is left out of these examples.
            //
            // M*                     - pointer to member
            // Ms*...                 - list of 0 or more pointer to member
            // T                      - Member<Ms*...> or NestedMember<MemberList<Ts...>, M*>
            // Ts...                  - list of 0 or more T
            // Resolved...            - list of 0 or more Member<Ms*...>
            // Remaining...           - same as Ts...
            //
            // rule0 - place fully resolved Member into tuple
            //
            //     matches
            //     std::tuple<Resolved...>, Member<Ms*...>, Remaining...
            //
            //     continues with
            //     std::tuple<Resolved..., Member<Ms*...>>, Remaining...
            //
            // rule1 - extract NestedMember into MemberChain
            //
            //     matches
            //     std::tuple<Resolved...>, NestedMember<MemberList<Ts...>, M*>, Remaining...
            //
            //     continues with
            //     std::tuple<Resolved...>, MemberChain<Member<M*>, Ts...>, Remaining...
            //
            // rule2 - extract resolved Member from MemberChain
            //
            //     matches
            //     std::tuple<Resolved...>, MemberChain<Member<M0s*...>, Member<M1s*...>, Ts...>, Remaining...
            //
            //     continues with
            //     std::tuple<Resolved...>, Member<M0s*..., M1s*...>, MemberChain<Member<M0s*...>, Ts...>, Remaining...
            //
            // rule3 - unfold NestedMember
            //
            //     matches
            //     std::tuple<Resolved...>, MemberChain<Member<M1s*...>, NestedMember<MemberList<T1s...>, M2*>, Ts...>, Remaining...
            //
            //     continues with
            //     std::tuple<Resolved...>, MemberChain<Member<M1s*..., M2*>, T1s...>..., MemberChain<Member<M1s*...>, Ts...>, Remaining...
            //
            // rule4 - drop empty MemberChain
            //
            //     matches
            //     std::tuple<Resolved...>, MemberChain<Member<Ms*...>>, Remaining...
            //
            //     continues with
            //     std::tuple<Resolved...>, Remaining...
            //
            // rule5 - terminate and return resolved types
            //
            //     match
            //     std::tuple<Resolved...>
            //
            //
            //
            //
            //
            //
            //
            // struct row
            // {
            //     float x;
            //     float y;
            // };
            // struct matrix
            // {
            //     row row0;
            //     row row1;
            // };
            // struct object
            // {
            //     matrix m;
            // };
            //
            // std::tuple<>, NestedMember<MemberList<NestedMember<MemberList<Member<&x>, Member<&y>>, &row0>, NestedMember<MemberList<Member<&x>, Member<&y>>, &row1>, &m>
            // rule1
            // std::tuple<>, MemberChain<Member<&m>, NestedMember<MemberList<Member<&x>, Member<&y>>, &row0>, NestedMember<MemberList<Member<&x>, Member<&y>>, &row1>>
            // rule3
            // std::tuple<>, MemberChain<Member<&m, &row0>, Member<&x>, Member<&y>>, MemberChain<Member<&m>, NestedMember<MemberList<Member<&x>, Member<&y>>, &row1>>
            // rule2
            // std::tuple<>, Member<&m, &row0, &x>, MemberChain<Member<&m, &row0>, Member<&y>>, MemberChain<Member<&m>, NestedMember<MemberList<Member<&x>, Member<&y>>, &row1>>
            // rule0
            // std::tuple<Member<&m, &row0, &x>>, MemberChain<Member<&m, &row0>, Member<&y>>, MemberChain<Member<&m>, NestedMember<MemberList<Member<&x>, Member<&y>>, &row1>>
            // rule2
            // std::tuple<Member<&m, &row0, &x>>, Member<&m, &row0, &y>, MemberChain<Member<&m, &row0>>, MemberChain<Member<&m>, NestedMember<MemberList<Member<&x>, Member<&y>>, &row1>>
            // rule0
            // std::tuple<Member<&m, &row0, &x>, Member<&m, &row0, &y>>, MemberChain<Member<&m, &row0>>, MemberChain<Member<&m>, NestedMember<MemberList<Member<&x>, Member<&y>>, &row1>>
            // rule4
            // std::tuple<Member<&m, &row0, &x>, Member<&m, &row0, &y>>, MemberChain<Member<&m>, NestedMember<MemberList<Member<&x>, Member<&y>>, &row1>>
            // rule3
            // std::tuple<Member<&m, &row0, &x>, Member<&m, &row0, &y>>, MemberChain<Member<&m, &row1>, Member<&x>, Member<&y>>
            // rule2
            // std::tuple<Member<&m, &row0, &x>, Member<&m, &row0, &y>>, Member<&m, &row1, &x>, MemberChain<Member<&m, &row1>, Member<&y>>
            // rule0
            // std::tuple<Member<&m, &row0, &x>, Member<&m, &row0, &y>, Member<&m, &row1, &x>>, MemberChain<Member<&m, &row1>, Member<&y>>
            // rule2
            // std::tuple<Member<&m, &row0, &x>, Member<&m, &row0, &y>, Member<&m, &row1, &x>>, Member<&m, &row1, &y>, MemberChain<Member<&m, &row1>>
            // rule0
            // std::tuple<Member<&m, &row0, &x>, Member<&m, &row0, &y>, Member<&m, &row1, &x>, Member<&m, &row1, &y>>, MemberChain<Member<&m, &row1>>
            // rule4
            // std::tuple<Member<&m, &row0, &x>, Member<&m, &row0, &y>, Member<&m, &row1, &x>, Member<&m, &row1, &y>>
            // rule5
            // DONE
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
                auto f = [&]<MemberName Name, auto M, typename... Us>(NestedMember<Name, MemberList<Us...>, M>)
                {
                    return getImpl<Resolved, MemberChain<Member<Name, M>, Us...>, Ts...>();
                };
                return f(T());
            }

            // rule2 && rule3 && rule4
            template<typename Resolved, is_member_chain T, typename... Remaining>
            static auto getImpl()
            {
                // rule4
                if constexpr (T::size == 0) { return getImpl<Resolved, Remaining...>(); }
                else
                {
                    auto f = [&]<typename M, typename U, typename... Us>(MemberChain<M, U, Us...>) {
                        // rule2
                        if constexpr (is_member<U>)
                        {
                            return getImpl<Resolved, concat_t<M, U>, MemberChain<M, Us...>, Remaining...>();
                        }
                        // rule3
                        else if constexpr (is_nested_member<U>)
                        {
                            auto g = [&]<MemberName Name, auto NM, typename... Xs>(
                              NestedMember<Name, MemberList<Xs...>, NM>)
                            {
                                using c0 = MemberChain<concat_t<M, Member<Name, NM>>, Xs...>;
                                using c1 = MemberChain<M, Us...>;
                                return getImpl<Resolved, c0, c1, Remaining...>();
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
                constexpr auto f = []<typename... Ts>(std::tuple<Ts...>) { return TypeDescriptor<Ts...>(); };
                return f(Resolved());
            }
        };
    }  // namespace detail

    template<typename... Ts>
    using GenerateTypeDescriptor = decltype(detail::GenerateTypeDescriptorImpl::get<Ts...>());
}  // namespace alex