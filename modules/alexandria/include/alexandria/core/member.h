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
    }  // namespace detail

    template<auto M, auto... Ms>
    class Member
    {
    public:
        // TODO: Constrain M and Ms... to be sequence of member pointers.

        inline static constexpr bool is_member_v = true;
        inline static constexpr auto mp          = detail::get_last_mp<M, Ms...>::value;
        using class_t                            = member_pointer_class_t<std::decay_t<decltype(M)>>;
        using value_t                            = member_pointer_value_t<std::decay_t<decltype(mp)>>;

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

    template<typename T>
    concept is_member = requires { T::is_member_v; };
}  // namespace alex