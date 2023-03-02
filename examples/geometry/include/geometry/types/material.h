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

struct Material
{
    alex::InstanceId id;
    std::string      name;
    float3           color;
    float            specular = 0;

    using descriptor_t   = alex::GenerateTypeDescriptor<alex::Member<&Material::id>,
                                                      alex::Member<&Material::name>,
                                                      alex::NestedMember<float3_t, &Material::color>,
                                                      alex::Member<&Material::specular>>;
    using get_query_t    = alex::GetQuery<descriptor_t>;
    using insert_query_t = alex::InsertQuery<descriptor_t>;
};
