#pragma once

namespace alex
{

    template<typename T, auto M>  //requires(T::is_member_list_v)
    class NestedMember
    {
    public:
        inline static constexpr bool is_nested_member_v = true;
    };

    template<typename T>
    concept is_nested_member = requires { T::is_nested_member_v; };
}  // namespace alex
