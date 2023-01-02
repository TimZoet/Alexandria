#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <vector>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria/core/library.h"
#include "bettertest/mixins/compare_mixin.h"
#include "bettertest/mixins/exception_mixin.h"
#include "bettertest/tests/unit_test.h"

namespace utils
{
    class LibraryMember : public bt::UnitTest<LibraryMember, bt::CompareMixin, bt::ExceptionMixin>
    {
    public:
        LibraryMember();

        ~LibraryMember() noexcept override;

        void reopen();

        void checkTypeTables(const std::vector<alex::NamespaceRow>& namespaces,
                             const std::vector<alex::TypeRow>&      types,
                             const std::vector<alex::PropertyRow>&  properties);

    protected:
        alex::LibraryPtr library;
        alex::Namespace* nameSpace = nullptr;
    };
}  // namespace utils