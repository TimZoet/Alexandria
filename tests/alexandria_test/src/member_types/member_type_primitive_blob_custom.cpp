#include "alexandria_test/member_types/member_type_primitive_blob_custom.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <array>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria/member_types/primitive_blob.h"

namespace
{
    template<size_t I>
    class FixedSizeFloatBlob
    {
    public:
        using value_t = float;

        FixedSizeFloatBlob() = default;

        ~FixedSizeFloatBlob() noexcept = default;

        FixedSizeFloatBlob(const FixedSizeFloatBlob&) = default;

        FixedSizeFloatBlob(FixedSizeFloatBlob&&) noexcept = default;

        FixedSizeFloatBlob& operator=(const FixedSizeFloatBlob&) = default;

        FixedSizeFloatBlob& operator=(FixedSizeFloatBlob&&) = default;

        void set(const std::vector<float>& v)
        {
            // Copy up to I values into array.
            for (size_t i = 0, j = std::min(I, v.size()); i < j; i++) value[i] = v[i];
            // Zero out the rest.
            for (size_t i = std::min(I, v.size()); i < I; ++i) value[i] = 0;
        }

        [[nodiscard]] operator sql::StaticBlob() const
        {
            return sql::StaticBlob{.data = static_cast<const void*>(value.data()),
                                   .size = value.size() * sizeof value_t};
        }

        [[nodiscard]] operator sql::Blob() const { return sql::toBlob(value); }

    private:
        std::array<float, I> value{};
    };
}  // namespace

template<size_t I>
struct alex::_is_primitive_blob<FixedSizeFloatBlob<I>> : std::true_type
{
};

void MemberTypePrimitiveBlobCustom::operator()()
{
    compareTrue(alex::_is_primitive_blob<FixedSizeFloatBlob<32>>::value);
    compareTrue(alex::is_primitive_blob<FixedSizeFloatBlob<32>>);
}
