#include "alexandria_test/member_types/member_type_primitive_array.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria/member_types/primitive_array.h"

void MemberTypePrimitiveArray::operator()()
{
    // PrimitiveArray with float.
    {
        alex::PrimitiveArray<float> prim;
        expectNoThrow([&] {
            prim.get().push_back(1.0f);
            prim.get().push_back(2.0f);
        });
        compareEQ(static_cast<size_t>(2), prim.get().size());
    }

    // PrimitiveArray with int.
    {
        alex::PrimitiveArray<int64_t> prim;
        expectNoThrow([&] {
            prim.get().push_back(1);
            prim.get().push_back(2);
        });
        compareEQ(static_cast<size_t>(2), prim.get().size());
    }
}
