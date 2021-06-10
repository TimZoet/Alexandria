#include "geometry/types.h"

float2 average(const float2& lhs, const float2& rhs) { return float2{(lhs.x + rhs.x) * 0.5f, (lhs.y + rhs.y) * 0.5f}; }

float3 average(const float3& lhs, const float3& rhs)
{
    return float3{(lhs.x + rhs.x) * 0.5f, (lhs.y + rhs.y) * 0.5f, (lhs.z + rhs.z) * 0.5f};
}

Vertex average(const Vertex& lhs, const Vertex& rhs)
{
    return Vertex{average(lhs.position, rhs.position), average(lhs.normal, rhs.normal), average(lhs.uv, rhs.uv)};
}
