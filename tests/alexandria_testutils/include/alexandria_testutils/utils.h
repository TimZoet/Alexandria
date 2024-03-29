#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <vector>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria-core/library.h"
#include "alexandria-core/namespace.h"
#include "bettertest/mixins/compare_mixin.h"
#include "bettertest/mixins/exception_mixin.h"
#include "bettertest/tests/unit_test.h"

namespace utils
{
    class LibraryMember : public bt::UnitTest<LibraryMember, bt::CompareMixin, bt::ExceptionMixin>
    {
    public:
        explicit LibraryMember(bool inMem = true);

        LibraryMember(const LibraryMember&) = delete;

        LibraryMember(LibraryMember&&) noexcept = delete;

        ~LibraryMember() noexcept override;

        LibraryMember& operator=(const LibraryMember&) = delete;

        LibraryMember& operator=(LibraryMember&&) noexcept = delete;

        void reopen();

        void checkTypeTables(const std::vector<alex::NamespaceRow>& namespaces,
                             const std::vector<alex::TypeRow>&      types,
                             const std::vector<alex::PropertyRow>&  properties,
                             const std::vector<alex::TableRow>&     tables);

    protected:
        alex::LibraryPtr library;
        bool             inMemory;
        alex::Namespace* nameSpace = nullptr;
    };
}  // namespace utils