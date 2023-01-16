#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

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
     * \brief Wrapper class that can hold various types of objects to be inserted as binary data.
     */
    template<typename...>
    class Blob;

    /**
     * \brief Blob specialization for single objects.
     * \tparam T Object type.
     */
    template<typename T>
        requires(std::is_trivially_copyable_v<T>)
    class Blob<T>
    {
    public:
        using value_t = T;

        Blob() = default;

        ~Blob() noexcept = default;

        Blob(const Blob&) = default;

        Blob(Blob&&) noexcept = default;

        Blob& operator=(const Blob&) = default;

        Blob& operator=(Blob&&) = default;

        /**
         * \brief Get value.
         * \return Value.
         */
        [[nodiscard]] value_t& get() noexcept { return value; }

        /**
         * \brief Get value.
         * \return Value.
         */
        [[nodiscard]] const value_t& get() const noexcept { return value; }

        template<typename U>
            requires(std::same_as<value_t, std::remove_cvref_t<U>>)
        void set(U&& v)
        {
            value = std::forward<U>(v);
        }

        /**
         * \brief Convert to sql::StaticBlob for binding.
         */
        [[nodiscard]] operator sql::StaticBlob() const { return sql::toStaticBlob(value); }

    private:
        value_t value;
    };

    /**
     * \brief Blob specialization for std::vector.
     * \tparam T Vector element type.
     */
    template<typename T>
        requires(std::is_trivially_copyable_v<T>)
    class Blob<std::vector<T>>
    {
    public:
        using value_t = std::vector<T>;

        Blob() = default;

        ~Blob() noexcept = default;

        Blob(const Blob&) = default;

        Blob(Blob&&) noexcept = default;

        Blob& operator=(const Blob&) = default;

        Blob& operator=(Blob&&) = default;

        /**
         * \brief Get vector of values.
         * \return Vector of values.
         */
        [[nodiscard]] value_t& get() noexcept { return value; }

        /**
         * \brief Get vector of values.
         * \return Vector of values.
         */
        [[nodiscard]] const value_t& get() const noexcept { return value; }

        template<typename U>
            requires(std::same_as<value_t, std::remove_cvref_t<U>>)
        void set(U&& v)
        {
            value = std::forward<U>(v);
        }

        /**
         * \brief Convert to sql::StaticBlob for binding.
         */
        [[nodiscard]] operator sql::StaticBlob() const noexcept { return sql::toStaticBlob(value); }

    private:
        value_t value;
    };

    ////////////////////////////////////////////////////////////////
    // Type traits.
    ////////////////////////////////////////////////////////////////

    template<typename...>
    struct _is_blob : std::false_type
    {
    };

    template<typename T>
    struct _is_blob<Blob<T>> : std::true_type
    {
    };

    // clang-format off
    template<typename T>
    concept is_blob = _is_blob<T>::value &&
                      (std::convertible_to<T, sql::StaticBlob> ||
                       std::convertible_to<T, sql::TransientBlob> ||
                       std::convertible_to<T, sql::Blob>) &&
        requires (T blob)
    {
        typename T::value_t;
        {blob.set(std::declval<typename T::value_t>())};
    };
    // clang-format on

    template<auto M>
    concept is_blob_mp = is_blob<member_pointer_value_t<decltype(M)>>;
}  // namespace alex
