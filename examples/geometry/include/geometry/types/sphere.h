#pragma once

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria/core/member.h"
#include "alexandria/core/type_descriptor.h"
#include "alexandria/member_types/instance_id.h"
#include "alexandria/queries/get_query.h"
#include "alexandria/queries/insert_query.h"

struct Sphere
{
    alex::InstanceId id;
    std::string      name;
    float            radius = 0;

    using descriptor_t = alex::
      GenerateTypeDescriptor<alex::Member<&Sphere::id>, alex::Member<&Sphere::name>, alex::Member<&Sphere::radius>>;
    using get_query_t    = alex::GetQuery<descriptor_t>;
    using insert_query_t = alex::InsertQuery<descriptor_t>;
};
