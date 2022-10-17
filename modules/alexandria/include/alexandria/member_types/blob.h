#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <type_traits>
#include <vector>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "common/static_assert.h"
#include "common/type_traits.h"
#include "cppql-core/binding.h"

namespace alex
{
    /**
     * \brief Wrapper class that can hold various types of objects to be inserted as binary data.
     */
    template<typename...>
    class Blob;

    /**
     * \brief Blob specialization for single objects.
     * \tparam E Object type.
     */
    template<typename E>
    class Blob<E>
    {
    public:
        using blob_t = E;

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
        [[nodiscard]] E& get() noexcept { return value; }

        /**
         * \brief Get const value.
         * \return Const value.
         */
        [[nodiscard]] const E& get() const noexcept { return value; }

        void set(blob_t& v)
        {
            // Move, copy, or generate static assertion failure.
            if constexpr (std::is_move_assignable_v<blob_t>)
                value = std::move(v);
            else if constexpr (std::is_copy_assignable_v<blob_t>)
                value = v;
            else
                constexpr_static_assert();
        }

        [[nodiscard]] sql::StaticBlob getStaticBlob() const noexcept
        {
            return sql::StaticBlob{.data = &value, .size = sizeof(E)};
        }

    private:
        E value;
    };

    /**
     * \brief Blob specialization for std::vector.
     * \tparam E Vector element type.
     */
    template<typename E>
    class Blob<std::vector<E>>
    {
    public:
        using blob_t = std::vector<E>;

        Blob() = default;

        ~Blob() noexcept = default;

        Blob(const Blob&) = default;

        Blob(Blob&&) noexcept = default;

        Blob& operator=(const Blob&) = default;

        Blob& operator=(Blob&&) = default;

        /**
         * \brief Get vector.
         * \return Vector.
         */
        [[nodiscard]] std::vector<E>& get() noexcept { return value; }

        /**
         * \brief Get const vector.
         * \return Const vector.
         */
        [[nodiscard]] const std::vector<E>& get() const noexcept { return value; }

        void set(blob_t& v)
        {
            // Move, copy, or generate static assertion failure.
            if constexpr (std::is_move_assignable_v<blob_t>)
                value = std::move(v);
            else if constexpr (std::is_copy_assignable_v<blob_t>)
                value = v;
            else
                constexpr_static_assert();
        }

        [[nodiscard]] sql::StaticBlob getStaticBlob() const noexcept
        {
            return sql::StaticBlob{.data = value.data(), .size = value.size() * sizeof(E)};
        }

    private:
        std::vector<E> value;
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
