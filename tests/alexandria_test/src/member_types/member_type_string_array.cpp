#include "alexandria_test/member_types/member_type_string_array.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria/member_types/string_array.h"

void MemberTypeStringArray::operator()()
{
    alex::StringArray strings;
    expectNoThrow([&] {
        strings.get().push_back("abc");
        strings.get().push_back("def");
    });
    compareEQ(static_cast<size_t>(2), strings.get().size());
}
