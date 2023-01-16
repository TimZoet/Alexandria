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
        using value_t = T;

        PrimitiveArray() = default;

        ~PrimitiveArray() noexcept = default;

        PrimitiveArray(const PrimitiveArray&) = default;

        PrimitiveArray(PrimitiveArray&&) noexcept = default;

        PrimitiveArray& operator=(const PrimitiveArray&) = default;

        PrimitiveArray& operator=(PrimitiveArray&&) noexcept = default;

        [[nodiscard]] std::vector<value_t>& get() noexcept { return values; }

        [[nodiscard]] const std::vector<value_t>& get() const noexcept { return values; }

        /**
         * \brief Clear vector.
         */
        void clear() { values.clear(); }

        template<typename U>
            requires(std::same_as<value_t, std::remove_cvref_t<U>>)
        void add(U&& v)
        {
            values.push_back(std::forward<U>(v));
        }

        auto begin() noexcept { return values.begin(); }

        auto end() noexcept { return values.end(); }

        [[nodiscard]] auto cbegin() const noexcept { return values.cbegin(); }

        [[nodiscard]] auto cend() const noexcept { return values.cend(); }

    private:
        std::vector<value_t> values;
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

    // clang-format off
    template<typename T>
    concept is_primitive_array = _is_primitive_array<T>::value && 
        requires(T array)
    {
        typename T::value_t;
        {array.begin()};
        {array.end()};
        {array.clear()};
        {array.add(std::declval<typename T::value_t>())};
    };
    // clang-format on

    template<auto M>
    concept is_primitive_array_mp = is_primitive_array<member_pointer_value_t<decltype(M)>>;
}  // namespace alex
