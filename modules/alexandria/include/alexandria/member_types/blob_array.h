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
        using element_t = T;
        using value_t   = std::vector<element_t>;

        BlobArray() = default;

        ~BlobArray() noexcept = default;

        BlobArray(const BlobArray&) = default;

        BlobArray(BlobArray&&) noexcept = default;

        BlobArray& operator=(const BlobArray&) = default;

        BlobArray& operator=(BlobArray&&) = default;

        value_t& get() noexcept { return values; }

        const value_t& get() const noexcept { return values; }

        /**
         * \brief Get number of elements.
         * \return Number of elements.
         */
        [[nodiscard]] size_t size() const noexcept { return values.size(); }

        [[nodiscard]] sql::StaticBlob getStaticBlob(const size_t index) const noexcept
        {
            return sql::StaticBlob{.data = &values[index], .size = sizeof element_t};
        }

    private:
        value_t values;
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
        using element_t = std::vector<T>;
        using value_t   = std::vector<element_t>;

        BlobArray() = default;

        ~BlobArray() noexcept = default;

        BlobArray(const BlobArray&) = default;

        BlobArray(BlobArray&&) noexcept = default;

        BlobArray& operator=(const BlobArray&) = default;

        BlobArray& operator=(BlobArray&&) = default;

        value_t& get() noexcept { return values; }

        const value_t& get() const noexcept { return values; }

        /**
         * \brief Get number of elements.
         * \return Number of elements.
         */
        [[nodiscard]] size_t size() const noexcept { return values.size(); }

        [[nodiscard]] sql::StaticBlob getStaticBlob(const size_t index) const noexcept
        {
            return sql::StaticBlob{.data = values[index].data(), .size = values[index].size() * sizeof T};
        }

    private:
        value_t values;
    };

    ////////////////////////////////////////////////////////////////
    // Type traits.
    ////////////////////////////////////////////////////////////////

    template<typename T>
    struct _is_blob_array : std::false_type
    {
    };

    template<typename T>
    struct _is_blob_array<BlobArray<T>> : std::true_type
    {
    };

    template<typename T>
    concept is_blob_array = _is_blob_array<T>::value;

    template<auto M>
    concept is_blob_array_mp = is_blob_array<member_pointer_value_t<decltype(M)>>;
}  // namespace alex
