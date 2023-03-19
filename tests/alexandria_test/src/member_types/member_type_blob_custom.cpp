#include "alexandria_test/member_types/member_type_blob_custom.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <array>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria-core/member_types/blob.h"

namespace
{
    class DoubleArrayBlob
    {
    public:
        using value_t = std::vector<int32_t>;

        DoubleArrayBlob() = default;

        ~DoubleArrayBlob() noexcept = default;

        DoubleArrayBlob(const DoubleArrayBlob&) = default;

        DoubleArrayBlob(DoubleArrayBlob&&) noexcept = default;

        DoubleArrayBlob& operator=(const DoubleArrayBlob&) = default;

        DoubleArrayBlob& operator=(DoubleArrayBlob&&) = default;

        void set(const std::vector<int32_t>& v)
        {
            if (v.size() != value0.size() + value1.size()) throw std::runtime_error("");
            std::memcpy(value0.data(), v.data(), sizeof value0);
            std::memcpy(value1.data(), v.data() + sizeof value0, sizeof value1);
        }

        [[nodiscard]] explicit operator sql::Blob() const& noexcept
        {
            auto* x = new int32_t[value0.size() + value1.size()];
            std::memcpy(x, value0.data(), sizeof value0);
            std::memcpy(x + sizeof value0, value1.data(), sizeof value1);
            return sql::Blob{.data = x, .size = sizeof value0 + sizeof value1, .destructor = [](void* ptr) {
                                 delete[] static_cast<int32_t*>(ptr);
                             }};
        }

    private:
        std::array<int32_t, 16> value0{};
        std::array<int32_t, 32> value1{};
    };
}  // namespace

template<>
struct alex::_is_blob<DoubleArrayBlob> : std::true_type
{
};

void MemberTypeBlobCustom::operator()()
{
    compareTrue(alex::_is_blob<DoubleArrayBlob>::value);
    compareTrue(alex::is_blob<DoubleArrayBlob>);
}
