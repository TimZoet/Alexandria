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
    template<typename E>
    requires(std::floating_point<E> || std::integral<E>) class PrimitiveArray
    {
    public:
        using element_t = E;

        PrimitiveArray() = default;

        ~PrimitiveArray() noexcept = default;

        PrimitiveArray(const PrimitiveArray&) = default;

        PrimitiveArray(PrimitiveArray&&) noexcept = default;

        PrimitiveArray& operator=(const PrimitiveArray&) = default;

        PrimitiveArray& operator=(PrimitiveArray&&) = default;

        /**
         * \brief Get vector.
         * \return Vector.
         */
        [[nodiscard]] std::vector<E>& get() noexcept { return container; }

        /**
         * \brief Get const vector.
         * \return Const vector.
         */
        [[nodiscard]] const std::vector<E>& get() const noexcept { return container; }

    private:
        std::vector<E> container;
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
