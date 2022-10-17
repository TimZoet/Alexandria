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

namespace alex
{
    template<typename T>
    concept is_string = std::same_as<std::string, T>;

    template<auto M>
    concept is_string_mp = is_string<member_pointer_value_t<decltype(M)>>;
}  // namespace alex
