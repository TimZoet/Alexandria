#pragma once

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria/core/member.h"
#include "alexandria/core/type_descriptor.h"
#include "alexandria/member_types/instance_id.h"
#include "alexandria/member_types/reference.h"
#include "alexandria/queries/get_query.h"
#include "alexandria/queries/insert_query.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "geometry/types/cube.h"
#include "geometry/types/float3.h"
#include "geometry/types/material.h"
#include "geometry/types/mesh.h"
#include "geometry/types/sphere.h"

struct Node
{
    alex::InstanceId          id;
    std::string               name;
    float3                    translation;
    alex::Reference<Material> material;
    alex::Reference<Cube>     cube;
    alex::Reference<Mesh>     mesh;
    alex::Reference<Sphere>   sphere;

    using descriptor_t   = alex::GenerateTypeDescriptor<alex::Member<&Node::id>,
                                                      alex::Member<&Node::name>,
                                                      alex::NestedMember<float3_t, &Node::translation>,
                                                      alex::Member<&Node::material>,
                                                      alex::Member<&Node::cube>,
                                                      alex::Member<&Node::mesh>,
                                                      alex::Member<&Node::sphere>>;
    using get_query_t    = alex::GetQuery<descriptor_t>;
    using insert_query_t = alex::InsertQuery<descriptor_t>;
};
