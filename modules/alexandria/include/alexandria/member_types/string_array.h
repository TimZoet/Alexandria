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
        using value_t   = std::vector<element_t>;

        StringArray() = default;

        ~StringArray() noexcept = default;

        StringArray(const StringArray&) = default;

        StringArray(StringArray&&) noexcept = default;

        StringArray& operator=(const StringArray&) = default;

        StringArray& operator=(StringArray&&) = default;

        value_t& get() noexcept { return container; }

        const value_t& get() const noexcept { return container; }

    private:
        value_t container;
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
