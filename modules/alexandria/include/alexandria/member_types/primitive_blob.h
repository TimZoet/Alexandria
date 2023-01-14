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
    template<typename T>
        requires(std::floating_point<T> || std::integral<T>)
    class PrimitiveBlob
    {
    public:
        using element_t = T;
        using value_t   = std::vector<element_t>;

        PrimitiveBlob() = default;

        ~PrimitiveBlob() noexcept = default;

        PrimitiveBlob(const PrimitiveBlob&) = default;

        PrimitiveBlob(PrimitiveBlob&&) noexcept = default;

        PrimitiveBlob& operator=(const PrimitiveBlob&) = default;

        PrimitiveBlob& operator=(PrimitiveBlob&&) = default;

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
    struct _is_primitive_blob : std::false_type
    {
    };

    template<typename T>
    struct _is_primitive_blob<PrimitiveBlob<T>> : std::true_type
    {
    };

    template<typename T>
    concept is_primitive_blob = _is_primitive_blob<T>::value;

    template<auto M>
    concept is_primitive_blob_mp = is_primitive_blob<member_pointer_value_t<decltype(M)>>;
}  // namespace alex
