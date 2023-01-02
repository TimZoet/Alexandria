#pragma once

namespace alex
{
    template<typename T, typename... Ts>  //requires(T::is_member_v || T::is_nested_member_v)
    class MemberList
    {
    public:
        inline static constexpr bool is_member_list_v = true;
    };
    
    template<typename T>
    concept is_member_list = requires
    {
        T::is_member_list_v;
    };
}  // namespace alex
