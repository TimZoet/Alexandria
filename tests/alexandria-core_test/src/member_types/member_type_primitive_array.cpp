#include "alexandria-core_test/member_types/member_type_primitive_array.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria-core/properties/primitive_array.h"

void MemberTypePrimitiveArray::operator()()
{
    compareTrue(alex::_is_primitive_array<alex::PrimitiveArray<float>>::value);
    compareTrue(alex::_is_primitive_array<alex::PrimitiveArray<uint64_t>>::value);
    compareTrue(alex::is_primitive_array<alex::PrimitiveArray<float>>);
    compareTrue(alex::is_primitive_array<alex::PrimitiveArray<uint64_t>>);
    compareTrue(std::same_as<float, alex::PrimitiveArray<float>::value_t>);
    compareTrue(std::same_as<int32_t, alex::PrimitiveArray<int32_t>::value_t>);
    compareTrue(requires(alex::PrimitiveArray<float> array) {
                    {
                        array.get()
                        } -> std::same_as<std::vector<float>&>;
                });
    compareTrue(requires(const alex::PrimitiveArray<float> array) {
                    {
                        array.get()
                        } -> std::same_as<const std::vector<float>&>;
                });
    compareTrue(requires(alex::PrimitiveArray<float> array) {
                    {
                        array.add(std::declval<float>())
                    };
                });
}
