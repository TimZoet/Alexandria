#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <concepts>
#include <string>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "common/type_traits.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "alexandria/member_types/instance_id.h"

namespace alex
{
    template<typename T>
    concept is_primitive = std::floating_point<T> || std::integral<T> || std::same_as<T, InstanceId>;

    template<auto M>
    concept is_primitive_mp = is_primitive<member_pointer_value_t<decltype(M)>>;
}  // namespace alex