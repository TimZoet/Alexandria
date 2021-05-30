#pragma once

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "bettertest/mixins/compare_mixin.h"
#include "bettertest/mixins/exception_mixin.h"
#include "bettertest/tests/unit_test.h"

class MemberTypePrimitiveArray : public bt::UnitTest<MemberTypePrimitiveArray, bt::CompareMixin, bt::ExceptionMixin>
{
public:
    void operator()() override;
};
