#pragma once

namespace alex
{
    template<typename M, typename... Ts>
    class MemberChain
    {
    public:
        inline static constexpr bool is_member_chain_v = true;
        inline static constexpr auto size              = sizeof...(Ts);
    };

    template<typename T>
    concept is_member_chain = requires
    {
        T::is_member_chain_v;
    };
}  // namespace alex
