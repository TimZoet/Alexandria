#include "alexandria_test/member_types/member_type_blob_array.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria-core/member_types/blob_array.h"

namespace
{
    struct Foo
    {
        float   x;
        int32_t y;
    };
}  // namespace

void MemberTypeBlobArray::operator()()
{
    compareTrue(alex::_is_blob_array<alex::BlobArray<Foo>>::value);
    compareTrue(alex::_is_blob_array<alex::BlobArray<std::vector<Foo>>>::value);
    compareTrue(alex::is_blob_array<alex::BlobArray<Foo>>);
    compareTrue(alex::is_blob_array<alex::BlobArray<std::vector<Foo>>>);
    compareTrue(std::same_as<Foo, alex::BlobArray<Foo>::value_t>);
    compareTrue(std::same_as<std::vector<Foo>, alex::BlobArray<std::vector<Foo>>::value_t>);
    compareTrue(requires(alex::BlobArray<Foo> blob) {
                    {
                        blob.get()
                        } -> std::same_as<std::vector<Foo>&>;
                });
    compareTrue(requires(const alex::BlobArray<Foo> blob) {
                    {
                        blob.get()
                        } -> std::same_as<const std::vector<Foo>&>;
                });
    compareTrue(requires(alex::BlobArray<std::vector<Foo>> blob) {
                    {
                        blob.get()
                        } -> std::same_as<std::vector<std::vector<Foo>>&>;
                });
    compareTrue(requires(const alex::BlobArray<std::vector<Foo>> blob) {
                    {
                        blob.get()
                        } -> std::same_as<const std::vector<std::vector<Foo>>&>;
                });
    compareTrue(requires(alex::BlobArray<Foo> blob) {
                    {
                        blob.add(std::declval<Foo>())
                    };
                });
    compareTrue(requires(alex::BlobArray<std::vector<Foo>> blob) {
                    {
                        blob.add(std::declval<std::vector<Foo>>())
                    };
                });
    compareTrue(requires(alex::BlobArray<std::vector<Foo>> blob) {
                    {
                        blob.getStaticBlob(std::declval<size_t>())
                        } -> std::same_as<sql::StaticBlob>;
                });
    compareTrue(requires(alex::BlobArray<Foo> blob) {
                    {
                        blob.getStaticBlob(std::declval<size_t>())
                        } -> std::same_as<sql::StaticBlob>;
                });
}
