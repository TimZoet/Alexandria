#include "alexandria_test/member_types/member_type_primitive_blob.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria/member_types/primitive_blob.h"

void MemberTypePrimitiveBlob::operator()()
{
    // PrimitiveBlob with float.
    {
        alex::PrimitiveBlob<float> blob;
        expectNoThrow([&] {
            blob.get().push_back(1.0f);
            blob.get().push_back(2.0f);
        });
        const auto staticBlob = blob.getStaticBlob();
        compareEQ(blob.get().data(), staticBlob.data);
        compareEQ(static_cast<size_t>(8), staticBlob.size);
    }

    // PrimitiveBlob with int.
    {
        alex::PrimitiveBlob<int64_t> blob;
        expectNoThrow([&] {
            blob.get().push_back(1);
            blob.get().push_back(2);
        });
        const auto staticBlob = blob.getStaticBlob();
        compareEQ(blob.get().data(), staticBlob.data);
        compareEQ(static_cast<size_t>(16), staticBlob.size);
    }
}
