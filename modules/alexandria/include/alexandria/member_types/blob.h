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

        value_t& get() noexcept { return value; }

        const value_t& get() const noexcept { return value; }

        template<typename U>
            requires(std::same_as<value_t, std::remove_cvref_t<U>>)
        void set(U&& v)
        {
            value = std::forward<U>(v);
        }

        // TODO: If this is an rvalue, the staticblob will refer deleted memory. Return TransientBlob instead?
        // Same issues for other specializations and for BlobArray type.
        [[nodiscard]] sql::StaticBlob getStaticBlob() const noexcept
        {
            return sql::StaticBlob{.data = &value, .size = sizeof value_t};
        }

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
        using element_t = T;
        using value_t   = std::vector<element_t>;

        Blob() = default;

        ~Blob() noexcept = default;

        Blob(const Blob&) = default;

        Blob(Blob&&) noexcept = default;

        Blob& operator=(const Blob&) = default;

        Blob& operator=(Blob&&) = default;

        value_t& get() noexcept { return value; }

        const value_t& get() const noexcept { return value; }

        template<typename U>
            requires(std::same_as<value_t, std::remove_cvref_t<U>>)
        void set(U&& v)
        {
            value = std::forward<U>(v);
        }

        [[nodiscard]] sql::StaticBlob getStaticBlob() const noexcept
        {
            return sql::StaticBlob{.data = value.data(), .size = value.size() * sizeof element_t};
        }

    private:
        value_t value;
    };

    ////////////////////////////////////////////////////////////////
    // Type traits.
    ////////////////////////////////////////////////////////////////

    template<typename T>
    struct _is_blob : std::false_type
    {
    };

    template<typename T>
    struct _is_blob<Blob<T>> : std::true_type
    {
    };

    template<typename T>
    concept is_blob = _is_blob<T>::value;

    template<auto M>
    concept is_blob_mp = is_blob<member_pointer_value_t<decltype(M)>>;
}  // namespace alex
