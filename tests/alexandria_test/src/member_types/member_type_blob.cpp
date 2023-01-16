#include "alexandria_test/member_types/member_type_blob.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria/member_types/blob.h"

namespace
{
    struct Foo
    {
        float   x;
        int32_t y;
    };
}  // namespace

void MemberTypeBlob::operator()()
{
    compareTrue(alex::_is_blob<alex::Blob<float>>::value);
    compareTrue(alex::_is_blob<alex::Blob<uint64_t>>::value);
    compareTrue(alex::is_blob<alex::Blob<float>>);
    compareTrue(alex::is_blob<alex::Blob<uint64_t>>);
    compareTrue(std::same_as<Foo, alex::Blob<Foo>::value_t>);
    compareTrue(std::same_as<std::vector<Foo>, alex::Blob<std::vector<Foo>>::value_t>);
    compareTrue(requires(alex::Blob<Foo> blob) {
                    {
                        blob.get()
                        } -> std::same_as<Foo&>;
                });
    compareTrue(requires(const alex::Blob<Foo> blob) {
                    {
                        blob.get()
                        } -> std::same_as<const Foo&>;
                });
    compareTrue(requires(alex::Blob<std::vector<Foo>> blob) {
                    {
                        blob.get()
                        } -> std::same_as<std::vector<Foo>&>;
                });
    compareTrue(requires(const alex::Blob<std::vector<Foo>> blob) {
                    {
                        blob.get()
                        } -> std::same_as<const std::vector<Foo>&>;
                });
    compareTrue(requires(alex::Blob<Foo> blob) {
                    {
                        blob.set(std::declval<Foo>())
                    };
                });
    compareTrue(requires(alex::Blob<std::vector<Foo>> blob) {
                    {
                        blob.set(std::declval<std::vector<Foo>>())
                    };
                });
    compareTrue(std::convertible_to<alex::Blob<Foo>, sql::StaticBlob>);
    compareTrue(std::convertible_to<alex::Blob<std::vector<Foo>>, sql::StaticBlob>);
}
