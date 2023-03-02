#pragma once

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria/core/member.h"
#include "alexandria/core/type_descriptor.h"
#include "alexandria/member_types/instance_id.h"
#include "alexandria/queries/get_query.h"
#include "alexandria/queries/insert_query.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "geometry/types/float3.h"

struct Cube
{
    alex::InstanceId id;
    std::string      name;
    float3           size;

    using descriptor_t   = alex::GenerateTypeDescriptor<alex::Member<&Cube::id>,
                                                      alex::Member<&Cube::name>,
                                                      alex::NestedMember<float3_t, &Cube::size>>;
    using get_query_t    = alex::GetQuery<descriptor_t>;
    using insert_query_t = alex::InsertQuery<descriptor_t>;
};
