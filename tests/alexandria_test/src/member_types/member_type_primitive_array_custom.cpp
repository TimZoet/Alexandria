#include "alexandria_test/member_types/member_type_primitive_array_custom.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <array>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria-core/properties/primitive_array.h"

namespace
{
    template<size_t I>
    class FixedSizeFloatArray
    {
    public:
        using value_t = float;

        FixedSizeFloatArray() = default;

        ~FixedSizeFloatArray() noexcept = default;

        FixedSizeFloatArray(const FixedSizeFloatArray&) = default;

        FixedSizeFloatArray(FixedSizeFloatArray&&) noexcept = default;

        FixedSizeFloatArray& operator=(const FixedSizeFloatArray&) = default;

        FixedSizeFloatArray& operator=(FixedSizeFloatArray&&) = default;

        void clear() { std::fill(values.begin(), values.end(), 0.0f); }

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
        std::array<float, I> values{};
    };
}  // namespace

template<size_t I>
struct alex::_is_primitive_array<FixedSizeFloatArray<I>> : std::true_type
{
};

void MemberTypePrimitiveArrayCustom::operator()()
{
    compareTrue(alex::_is_primitive_array<FixedSizeFloatArray<32>>::value);
    compareTrue(alex::is_primitive_array<FixedSizeFloatArray<32>>);
}
