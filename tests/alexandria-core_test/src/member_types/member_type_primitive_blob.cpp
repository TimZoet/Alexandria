#include "alexandria-core_test/member_types/member_type_primitive_blob.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria-core/properties/primitive_blob.h"

void MemberTypePrimitiveBlob::operator()()
{
    compareTrue(alex::_is_primitive_blob<alex::PrimitiveBlob<float>>::value);
    compareTrue(alex::_is_primitive_blob<alex::PrimitiveBlob<uint64_t>>::value);
    compareTrue(alex::is_primitive_blob<alex::PrimitiveBlob<float>>);
    compareTrue(alex::is_primitive_blob<alex::PrimitiveBlob<uint64_t>>);
    compareTrue(std::same_as<float, alex::PrimitiveBlob<float>::value_t>);
    compareTrue(std::same_as<int32_t, alex::PrimitiveBlob<int32_t>::value_t>);
    compareTrue(requires(alex::PrimitiveBlob<float> blob) {
                    {
                        blob.get()
                        } -> std::same_as<std::vector<float>&>;
                });
    compareTrue(requires(const alex::PrimitiveBlob<float> blob) {
                    {
                        blob.get()
                        } -> std::same_as<const std::vector<float>&>;
                });
    compareTrue(requires(alex::PrimitiveBlob<float> blob) {
                    {
                        blob.set(std::declval<std::vector<float>>())
                    };
                });
    compareTrue(explicitly_convertible_to<alex::PrimitiveBlob<float>, sql::StaticBlob>);
}