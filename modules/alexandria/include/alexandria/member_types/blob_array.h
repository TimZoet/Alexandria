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
#include "cppql-core/binding.h"

namespace alex
{
    /**
     * \brief Wrapper class that can hold various types of objects to be inserted as arrays of binary data.
     */
    template<typename...>
    class BlobArray;

    /**
     * \brief BlobArray specialization for single objects.
     * \tparam E Object type.
     */
    template<typename E>
    class BlobArray<E>
    {
    public:
        template<typename IdMember, typename... Members>
        friend class BlobArrayHandler;

        using blob_t = E;

        BlobArray() = default;

        ~BlobArray() noexcept = default;

        BlobArray(const BlobArray&) = default;

        BlobArray(BlobArray&&) noexcept = default;

        BlobArray& operator=(const BlobArray&) = default;

        BlobArray& operator=(BlobArray&&) = default;

        /**
         * \brief Get values.
         * \return Values.
         */
        [[nodiscard]] std::vector<E>& get() noexcept { return values; }

        /**
         * \brief Get const values.
         * \return Const values.
         */
        [[nodiscard]] const std::vector<E>& get() const noexcept { return values; }

        /**
         * \brief Get number of elements.
         * \return Number of elements.
         */
        [[nodiscard]] size_t size() const noexcept { return values.size(); }

        [[nodiscard]] sql::StaticBlob getStaticBlob(const size_t index) const noexcept
        {
            return sql::StaticBlob{.data = &values[index], .size = sizeof(E)};
        }

    private:
        std::vector<E> values;
    };

    /**
     * \brief BlobArray specialization for std::vector.
     * \tparam E Vector element type.
     */
    template<typename E>
    class BlobArray<std::vector<E>>
    {
    public:
        template<typename IdMember, typename... Members>
        friend class BlobArrayHandler;

        using blob_t = std::vector<E>;

        BlobArray() = default;

        ~BlobArray() noexcept = default;

        BlobArray(const BlobArray&) = default;

        BlobArray(BlobArray&&) noexcept = default;

        BlobArray& operator=(const BlobArray&) = default;

        BlobArray& operator=(BlobArray&&) = default;

        /**
         * \brief Get values.
         * \return Values.
         */
        [[nodiscard]] std::vector<std::vector<E>>& get() noexcept { return values; }

        /**
         * \brief Get const values.
         * \return Const values.
         */
        [[nodiscard]] const std::vector<std::vector<E>>& get() const noexcept { return values; }

        /**
         * \brief Get number of elements.
         * \return Number of elements.
         */
        [[nodiscard]] size_t size() const noexcept { return values.size(); }

        [[nodiscard]] sql::StaticBlob getStaticBlob(const size_t index) const noexcept
        {
            return sql::StaticBlob{.data = values[index].data(), .size = values[index].size() * sizeof(E)};
        }

    private:
        std::vector<std::vector<E>> values;
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
