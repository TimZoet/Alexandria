#include "alexandria_test/member_types/member_type_blob.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria/member_types/blob.h"

namespace
{
    struct Foo
    {
        int32_t x;
        int32_t y;
    };
}  // namespace

void MemberTypeBlob::operator()()
{
    // Blob with struct.
    {
        alex::Blob<Foo> blob;
        expectNoThrow([&] {
            blob.get().x = 10;
            blob.get().y = 20;
        });
        const auto staticBlob = blob.getStaticBlob();
        compareEQ(&blob.get(), staticBlob.data);
        compareEQ(static_cast<size_t>(8), staticBlob.size);
    }

    // Blob with vector of floats.
    {
        alex::Blob<std::vector<float>> blob;
        expectNoThrow([&] {
            blob.get().push_back(1.0f);
            blob.get().push_back(2.0f);
            blob.get().push_back(3.0f);
        });
        const auto staticBlob = blob.getStaticBlob();
        compareEQ(blob.get().data(), staticBlob.data);
        compareEQ(static_cast<size_t>(12), staticBlob.size);
    }

    // Blob with vector of structs.
    {
        alex::Blob<std::vector<Foo>> blob;
        expectNoThrow([&] {
            blob.get().emplace_back(11, 22);
            blob.get().emplace_back(33, 44);
        });
        const auto staticBlob = blob.getStaticBlob();
        compareEQ(blob.get().data(), staticBlob.data);
        compareEQ(static_cast<size_t>(16), staticBlob.size);
    }
}
