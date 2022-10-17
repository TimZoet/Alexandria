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

#include "common/static_assert.h"
#include "common/type_traits.h"
#include "cppql-core/binding.h"

namespace alex
{
    template<typename E>
    requires(std::floating_point<E> || std::integral<E>) class PrimitiveBlob
    {
    public:
        template<typename IdMember, typename... Members>
        friend class PrimitiveHandler;

        using blob_t = std::vector<E>;

        PrimitiveBlob() = default;

        ~PrimitiveBlob() noexcept = default;

        PrimitiveBlob(const PrimitiveBlob&) = default;

        PrimitiveBlob(PrimitiveBlob&&) noexcept = default;

        PrimitiveBlob& operator=(const PrimitiveBlob&) = default;

        PrimitiveBlob& operator=(PrimitiveBlob&&) = default;

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
