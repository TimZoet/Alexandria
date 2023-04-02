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
     * \brief Wrapper class that can hold various types of objects to be inserted as arrays of binary data.
     */
    template<typename...>
    class BlobArray;

    /**
     * \brief BlobArray specialization for single objects.
     * \tparam T Object type.
     */
    template<typename T>
        requires(std::is_trivially_copyable_v<T>)
    class BlobArray<T>
    {
    public:
        using value_t = T;

        BlobArray() = default;

        ~BlobArray() noexcept = default;

        BlobArray(const BlobArray&) = default;

        BlobArray(BlobArray&&) noexcept = default;

        BlobArray& operator=(const BlobArray&) = default;

        BlobArray& operator=(BlobArray&&) = default;

        [[nodiscard]] std::vector<value_t>& get() noexcept { return values; }

        [[nodiscard]] const std::vector<value_t>& get() const noexcept { return values; }

        /**
         * \brief Get number of elements.
         * \return Number of elements.
         */
        [[nodiscard]] size_t size() const noexcept { return values.size(); }

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

        [[nodiscard]] sql::StaticBlob getStaticBlob(const size_t index) const
        {
            return sql::toStaticBlob(values[index]);
        }

    private:
        std::vector<value_t> values;
    };

    /**
     * \brief BlobArray specialization for std::vector.
     * \tparam T Vector element type.
     */
    template<typename T>
        requires(std::is_trivially_copyable_v<T>)
    class BlobArray<std::vector<T>>
    {
    public:
        using value_t = std::vector<T>;

        BlobArray() = default;

        ~BlobArray() noexcept = default;

        BlobArray(const BlobArray&) = default;

        BlobArray(BlobArray&&) noexcept = default;

        BlobArray& operator=(const BlobArray&) = default;

        BlobArray& operator=(BlobArray&&) = default;

        [[nodiscard]] std::vector<value_t>& get() noexcept { return values; }

        [[nodiscard]] const std::vector<value_t>& get() const noexcept { return values; }

        /**
         * \brief Get number of elements.
         * \return Number of elements.
         */
        [[nodiscard]] size_t size() const noexcept { return values.size(); }

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

        [[nodiscard]] sql::StaticBlob getStaticBlob(const size_t index) const
        {
            return sql::toStaticBlob(values[index]);
        }

    private:
        std::vector<value_t> values;
    };

    ////////////////////////////////////////////////////////////////
    // Type traits.
    ////////////////////////////////////////////////////////////////

    template<typename...>
    struct _is_blob_array : std::false_type
    {
    };

    template<typename T>
    struct _is_blob_array<BlobArray<T>> : std::true_type
    {
    };

    // clang-format off
    template<typename T>
    concept is_blob_array = _is_blob_array<T>::value &&
                            (requires (T blob) { {blob.getStaticBlob(std::declval<size_t>())} -> std::same_as<sql::StaticBlob>; } || 
                             requires (T blob) { {blob.getTransientBlob(std::declval<size_t>())} -> std::same_as<sql::TransientBlob>; } ||
                             requires (T blob) { {blob.getBlob(std::declval<size_t>())} -> std::same_as<sql::Blob>; }) &&
        requires (T blob)
    {
        typename T::value_t;
        {blob.size()} -> std::same_as<size_t>;
        {blob.clear()};
        {blob.add(std::declval<typename T::value_t>())};
    };
    // clang-format on

    template<auto M>
    concept is_blob_array_mp = is_blob_array<member_pointer_value_t<decltype(M)>>;
}  // namespace alex
