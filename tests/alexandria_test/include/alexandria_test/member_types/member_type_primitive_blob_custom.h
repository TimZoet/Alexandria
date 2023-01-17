#pragma once

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "bettertest/mixins/compare_mixin.h"
#include "bettertest/mixins/exception_mixin.h"
#include "bettertest/tests/unit_test.h"

class MemberTypePrimitiveBlobCustom final
    : public bt::UnitTest<MemberTypePrimitiveBlobCustom, bt::CompareMixin, bt::ExceptionMixin>
{
public:
    void operator()() override;
};
