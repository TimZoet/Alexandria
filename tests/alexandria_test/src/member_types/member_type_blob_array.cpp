#include "alexandria_test/member_types/member_type_blob_array.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria/member_types/blob_array.h"

namespace
{
    struct Foo
    {
        int32_t x;
        int32_t y;
    };
}  // namespace

void MemberTypeBlobArray::operator()()
{
    // BlobArray with struct.
    {
        alex::BlobArray<Foo> blob;
        expectNoThrow([&] {
            blob.get().resize(2);
            blob.get()[0].x = 11;
            blob.get()[0].y = 22;
            blob.get()[1].x = 33;
            blob.get()[1].y = 44;
        });
        const auto staticBlob0 = blob.getStaticBlob(0);
        const auto staticBlob1 = blob.getStaticBlob(1);
        compareEQ(&blob.get()[0], staticBlob0.data);
        compareEQ(&blob.get()[1], staticBlob1.data);
        compareEQ(static_cast<size_t>(8), staticBlob0.size);
        compareEQ(static_cast<size_t>(8), staticBlob1.size);
    }

    // BlobArray with vector of floats.
    {
        alex::BlobArray<std::vector<float>> blob;
        expectNoThrow([&] {
            blob.get().resize(3);
            blob.get()[0].push_back(1.0f);
            blob.get()[0].push_back(2.0f);
            blob.get()[0].push_back(3.0f);
            blob.get()[1].push_back(4.0f);
            blob.get()[1].push_back(5.0f);
        });
        const auto staticBlob0 = blob.getStaticBlob(0);
        const auto staticBlob1 = blob.getStaticBlob(1);
        const auto staticBlob2 = blob.getStaticBlob(2);
        compareEQ(blob.get()[0].data(), staticBlob0.data);
        compareEQ(blob.get()[1].data(), staticBlob1.data);
        compareEQ(blob.get()[2].data(), staticBlob2.data);
        compareEQ(static_cast<size_t>(12), staticBlob0.size);
        compareEQ(static_cast<size_t>(8), staticBlob1.size);
        compareEQ(static_cast<size_t>(0), staticBlob2.size);
    }

    // BlobArray with vector of structs.
    {
        alex::BlobArray<std::vector<Foo>> blob;
        expectNoThrow([&] {
            blob.get().resize(2);
            blob.get()[0].emplace_back(11, 22);
            blob.get()[0].emplace_back(33, 44);
            blob.get()[1].emplace_back(55, 66);
        });
        const auto staticBlob0 = blob.getStaticBlob(0);
        const auto staticBlob1 = blob.getStaticBlob(1);
        compareEQ(blob.get()[0].data(), staticBlob0.data);
        compareEQ(blob.get()[1].data(), staticBlob1.data);
        compareEQ(static_cast<size_t>(16), staticBlob0.size);
        compareEQ(static_cast<size_t>(8), staticBlob1.size);
    }
}
