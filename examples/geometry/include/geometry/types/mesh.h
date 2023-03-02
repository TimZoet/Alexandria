#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <vector>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria/core/member.h"
#include "alexandria/core/type_descriptor.h"
#include "alexandria/member_types/blob.h"
#include "alexandria/member_types/instance_id.h"
#include "alexandria/queries/get_query.h"
#include "alexandria/queries/insert_query.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "geometry/types/float3.h"
#include "geometry/types/int3.h"

struct Mesh
{
    alex::InstanceId                id;
    std::string                     name;
    alex::Blob<std::vector<float3>> vertices;
    alex::Blob<std::vector<int3>>   indices;

    using descriptor_t   = alex::GenerateTypeDescriptor<alex::Member<&Mesh::id>,
                                                      alex::Member<&Mesh::name>,
                                                      alex::Member<&Mesh::vertices>,
                                                      alex::Member<&Mesh::indices>>;
    using get_query_t    = alex::GetQuery<descriptor_t>;
    using insert_query_t = alex::InsertQuery<descriptor_t>;
};
