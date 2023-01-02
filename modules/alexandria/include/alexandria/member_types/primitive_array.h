#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <concepts>
#include <vector>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "common/type_traits.h"

namespace alex
{
    template<typename T>
        requires(std::floating_point<T> || std::integral<T>)
    class PrimitiveArray
    {
    public:
        using element_t = T;
        using value_t   = std::vector<element_t>;

        PrimitiveArray() = default;

        ~PrimitiveArray() noexcept = default;

        PrimitiveArray(const PrimitiveArray&) = default;

        PrimitiveArray(PrimitiveArray&&) noexcept = default;

        PrimitiveArray& operator=(const PrimitiveArray&) = default;

        PrimitiveArray& operator=(PrimitiveArray&&) noexcept = default;

        template<typename Self>
        auto get(this Self&& self)
        {
            return std::forward<Self>(self).container;
        }

    private:
        value_t container;
    };

    ////////////////////////////////////////////////////////////////
    // Type traits.
    ////////////////////////////////////////////////////////////////

    template<typename T>
    struct _is_primitive_array : std::false_type
    {
    };

    template<typename T>
    struct _is_primitive_array<PrimitiveArray<T>> : std::true_type
    {
    };

    template<typename T>
    concept is_primitive_array = _is_primitive_array<T>::value;

    template<auto M>
    concept is_primitive_array_mp = is_primitive_array<member_pointer_value_t<decltype(M)>>;
}  // namespace alex
