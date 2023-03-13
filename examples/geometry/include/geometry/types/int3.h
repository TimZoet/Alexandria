#pragma once

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria/core/member.h"

struct int3
{
    int32_t x = 0;
    int32_t y = 0;
    int32_t z = 0;
};

using int3_t = alex::MemberList<alex::Member<"x", &int3::x>, alex::Member<"y", &int3::y>, alex::Member<"z", &int3::z>>;
