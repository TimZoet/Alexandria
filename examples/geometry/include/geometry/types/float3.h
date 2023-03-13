#pragma once

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria/core/member.h"

struct float3
{
    float x = 0;
    float y = 0;
    float z = 0;
};

using float3_t =
  alex::MemberList<alex::Member<"x", &float3::x>, alex::Member<"y", &float3::y>, alex::Member<"z", &float3::z>>;
