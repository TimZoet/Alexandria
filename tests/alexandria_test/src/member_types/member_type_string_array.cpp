#include "alexandria_test/member_types/member_type_string_array.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria-core/properties/string_array.h"

void MemberTypeStringArray::operator()()
{
    compareTrue(alex::_is_string_array<alex::StringArray>::value);
    compareTrue(alex::is_string_array<alex::StringArray>);
    compareTrue(std::same_as<std::string, alex::StringArray::value_t>);
    compareTrue(requires(alex::StringArray array) {
                    {
                        array.get()
                        } -> std::same_as<std::vector<std::string>&>;
                });
    compareTrue(requires(const alex::StringArray array) {
                    {
                        array.get()
                        } -> std::same_as<const std::vector<std::string>&>;
                });
    compareTrue(requires(alex::StringArray array) {
                    {
                        array.add(std::declval<std::string>())
                    };
                });
}
