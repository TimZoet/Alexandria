#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <string>
#include <vector>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "common/type_traits.h"

namespace alex
{
    class StringArray
    {
    public:
        using element_t = std::string;

        StringArray() = default;

        ~StringArray() noexcept = default;

        StringArray(const StringArray&) = default;

        StringArray(StringArray&&) noexcept = default;

        StringArray& operator=(const StringArray&) = default;

        StringArray& operator=(StringArray&&) = default;

        /**
         * \brief Get vector.
         * \return Vector.
         */
        [[nodiscard]] std::vector<std::string>& get() noexcept { return container; }

        /**
         * \brief Get const vector.
         * \return Const vector.
         */
        [[nodiscard]] const std::vector<std::string>& get() const noexcept { return container; }

    private:
        std::vector<std::string> container;
    };

    ////////////////////////////////////////////////////////////////
    // Type traits.
    ////////////////////////////////////////////////////////////////

    template<typename T>
    struct _is_string_array : std::false_type
    {
    };

    template<>
    struct _is_string_array<StringArray> : std::true_type
    {
    };

    template<typename T>
    concept is_string_array = _is_string_array<T>::value;

    template<auto M>
    concept is_string_array_mp = is_string_array<member_pointer_value_t<decltype(M)>>;
}  // namespace alex
