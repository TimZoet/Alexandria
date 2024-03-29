#pragma once

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "bettertest/mixins/compare_mixin.h"
#include "bettertest/mixins/exception_mixin.h"
#include "bettertest/tests/unit_test.h"

class CreateLibrary final : public bt::UnitTest<CreateLibrary, bt::CompareMixin, bt::ExceptionMixin>
{
public:
    static constexpr bool isParallel = false;

    void operator()() override;
};
