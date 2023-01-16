#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <concepts>
#include <type_traits>
#include <vector>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "common/type_traits.h"
#include "cppql/core/binding.h"

namespace alex
{
    /**
     * \brief Wrapper class that holds a std::vector of floating point or integral values
     * that are inserted as binary data.
     * \tparam T Element type.
     */
    template<typename T>
        requires(std::floating_point<T> || std::integral<T>)
    class PrimitiveBlob
    {
    public:
        using value_t = T;

        PrimitiveBlob() = default;

        ~PrimitiveBlob() noexcept = default;

        PrimitiveBlob(const PrimitiveBlob&) = default;

        PrimitiveBlob(PrimitiveBlob&&) noexcept = default;

        PrimitiveBlob& operator=(const PrimitiveBlob&) = default;

        PrimitiveBlob& operator=(PrimitiveBlob&&) = default;

        /**
         * \brief Get vector of values.
         * \return Vector of values.
         */
        [[nodiscard]] std::vector<value_t>& get() noexcept { return value; }

        /**
         * \brief Get vector of values.
         * \return Vector of values.
         */
        [[nodiscard]] const std::vector<value_t>& get() const noexcept { return value; }

        /**
         * \brief Set values.
         * \tparam U Vector type. Must be same as std::vector<T>.
         * \param v Vector.
         */
        template<typename U>
            requires(std::same_as<std::vector<value_t>, std::remove_cvref_t<U>>)
        void set(U&& v)
        {
            value = std::forward<U>(v);
        }

        /**
         * \brief Convert to sql::StaticBlob for binding.
         */
        [[nodiscard]] operator sql::StaticBlob() const { return sql::toStaticBlob(value); }

    private:
        std::vector<value_t> value;
    };

    ////////////////////////////////////////////////////////////////
    // Type traits.
    ////////////////////////////////////////////////////////////////


    template<typename...>
    struct _is_primitive_blob : std::false_type
    {
    };

    template<typename T>
    struct _is_primitive_blob<PrimitiveBlob<T>> : std::true_type
    {
    };

    // clang-format off
    template<typename T>
    concept is_primitive_blob = _is_primitive_blob<T>::value && 
                               (std::convertible_to<T, sql::StaticBlob> ||
                                std::convertible_to<T, sql::TransientBlob> ||
                                std::convertible_to<T, sql::Blob>) &&
        requires (T blob)
    {
        typename T::value_t;
        {blob.set(std::declval<std::vector<typename T::value_t>>())};
    } && (std::floating_point<typename T::value_t> || std::integral<typename T::value_t>);
    // clang-format on

    template<auto M>
    concept is_primitive_blob_mp = is_primitive_blob<member_pointer_value_t<decltype(M)>>;
}  // namespace alex
