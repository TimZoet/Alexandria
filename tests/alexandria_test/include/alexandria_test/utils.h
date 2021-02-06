#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <vector>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria/library.h"
#include "bettertest/mixins/compare_mixin.h"
#include "bettertest/mixins/exception_mixin.h"
#include "bettertest/tests/unit_test.h"

namespace utils
{
    struct Type
    {
        Type(int64_t _id, std::string _name) : id(_id), name(std::move(_name)) {}

        int64_t     id;
        std::string name;
    };

    struct Property
    {
        Property(int64_t     _id,
                 std::string _name,
                 std::string _dataType,
                 int64_t     _nestedType,
                 int32_t     _isReference,
                 int32_t     _isArray,
                 int32_t     _isBlob) :
            id(_id),
            name(std::move(_name)),
            nestedType(_nestedType),
            isReference(_isReference),
            isArray(_isArray),
            isBlob(_isBlob)
        {
            alex::fromString(_dataType, dataType);
        }

        int64_t        id;
        std::string    name;
        alex::DataType dataType;
        int64_t        nestedType;
        int32_t        isReference;
        int32_t        isArray;
        int32_t        isBlob;
    };

    struct Member
    {
        Member(int64_t _id, int64_t _type, int64_t _prop) : id(_id), type(_type), prop(_prop) {}

        int64_t id;
        int64_t type;
        int64_t prop;
    };

    class LibraryMember : public bt::UnitTest<LibraryMember, bt::CompareMixin, bt::ExceptionMixin>
    {
    public:
        LibraryMember();

        virtual ~LibraryMember() noexcept;

        void reopen();

        void checkTypeTables(std::vector<Type> types, std::vector<Property> properties, std::vector<Member> members);

    protected:
        alex::LibraryPtr library;
    };
}  // namespace utils