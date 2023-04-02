#include "alexandria_test/member_types/member_type_blob_array_custom.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <array>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria-core/properties/blob_array.h"

namespace
{
    struct Foo
    {
        uint64_t y;
        int8_t   z;
    };

    class FooBlobArray
    {
    public:
        using value_t = Foo;

        FooBlobArray() = default;

        ~FooBlobArray() noexcept = default;

        FooBlobArray(const FooBlobArray&) = default;

        FooBlobArray(FooBlobArray&&) noexcept = default;

        FooBlobArray& operator=(const FooBlobArray&) = default;

        FooBlobArray& operator=(FooBlobArray&&) = default;

        /**
         * \brief Get number of elements.
         * \return Number of elements.
         */
        [[nodiscard]] size_t size() const noexcept { return count; }

        void clear() { count = 0; }

        template<typename U>
            requires(std::same_as<value_t, std::remove_cvref_t<U>>)
        void add(U&& v)
        {
            if (count < values.size())
            {
                values[count] = std::forward<U>(v);
                count++;
            }
            else
                throw std::runtime_error("");
        }

        [[nodiscard]] sql::StaticBlob getStaticBlob(const size_t index) const
        {
            return sql::toStaticBlob(values[index]);
        }

    private:
        std::array<Foo, 16> values{};
        size_t              count = 0;
    };
}  // namespace

template<>
struct alex::_is_blob_array<FooBlobArray> : std::true_type
{
};

void MemberTypeBlobArrayCustom::operator()()
{
    compareTrue(alex::_is_blob_array<FooBlobArray>::value);
    compareTrue(alex::is_blob_array<FooBlobArray>);
}
