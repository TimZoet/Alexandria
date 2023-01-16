#include "alexandria_test/member_types/member_type_string_array_custom.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <array>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria/member_types/string_array.h"

namespace
{
    template<size_t I>
    class FixedSizeStringArray
    {
    public:
        using value_t = std::string;

        FixedSizeStringArray() = default;

        ~FixedSizeStringArray() noexcept = default;

        FixedSizeStringArray(const FixedSizeStringArray&) = default;

        FixedSizeStringArray(FixedSizeStringArray&&) noexcept = default;

        FixedSizeStringArray& operator=(const FixedSizeStringArray&) = default;

        FixedSizeStringArray& operator=(FixedSizeStringArray&&) = default;

        void clear() { std::fill(values.begin(), values.end(), ""); }

        template<typename U>
            requires(std::same_as<value_t, std::remove_cvref_t<U>>)
        void add(U&& v)
        {
            values.push_back(std::forward<U>(v));
        }

        auto begin() noexcept { return values.begin(); }

        auto end() noexcept { return values.end(); }

        [[nodiscard]] auto cbegin() const noexcept { return values.cbegin(); }

        [[nodiscard]] auto cend() const noexcept { return values.cend(); }

    private:
        std::array<std::string, I> values{};
    };
}  // namespace

template<size_t I>
struct alex::_is_string_array<FixedSizeStringArray<I>> : std::true_type
{
};

void MemberTypeStringArrayCustom::operator()()
{
    compareTrue(alex::_is_string_array<FixedSizeStringArray<32>>::value);
    compareTrue(alex::is_string_array<FixedSizeStringArray<32>>);
}
