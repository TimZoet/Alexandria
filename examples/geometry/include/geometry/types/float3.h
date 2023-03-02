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

using float3_t = alex::MemberList<alex::Member<&float3::x>, alex::Member<&float3::y>, alex::Member<&float3::z>>;
