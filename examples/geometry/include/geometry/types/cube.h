#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <format>
#include <iostream>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria/core/member.h"
#include "alexandria/core/type_descriptor.h"
#include "alexandria/member_types/instance_id.h"
#include "alexandria/queries/delete_query.h"
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
    using delete_query_t = alex::DeleteQuery<descriptor_t>;
    using get_query_t    = alex::GetQuery<descriptor_t>;

    friend std::ostream& operator<<(std::ostream& out, const Cube& obj)
    {
        return out << std::format("Cube{{name={}, size=[{}, {}, {}]}}", obj.name, obj.size.x, obj.size.y, obj.size.x);
    }
};

struct CubeInsertQuery final : alex::InsertQuery<Cube::descriptor_t>
{
    CubeInsertQuery() = delete;

    CubeInsertQuery(const CubeInsertQuery&) = delete;

    CubeInsertQuery(CubeInsertQuery&&) noexcept = delete;

    explicit CubeInsertQuery(type_descriptor_t desc) : InsertQuery(std::move(desc)) {}

    ~CubeInsertQuery() noexcept override = default;

    CubeInsertQuery& operator=(const CubeInsertQuery&) = delete;

    CubeInsertQuery& operator=(CubeInsertQuery&&) noexcept = delete;

    void operator()(object_t& instance) override
    {
        if (instance.name.empty()) throw std::runtime_error("Cannot insert cube without name.");
        if (instance.size.x <= 0 || instance.size.y <= 0 || instance.size.z <= 0)
            throw std::runtime_error("Cannot insert cube with size <= 0.");

        return InsertQuery::operator()(instance);
    }
};
