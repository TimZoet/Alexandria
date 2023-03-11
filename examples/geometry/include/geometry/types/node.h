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
#include "alexandria/member_types/reference.h"
#include "alexandria/queries/delete_query.h"
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
    using delete_query_t = alex::DeleteQuery<descriptor_t>;
    using get_query_t    = alex::GetQuery<descriptor_t>;

    friend std::ostream& operator<<(std::ostream& out, const Node& obj)
    {
        return out << std::format("Node{{name={}, color=[{}, {}, {}], material={}, cube={}, mesh={}, sphere={}}}",
                                  obj.name,
                                  obj.translation.x,
                                  obj.translation.y,
                                  obj.translation.z,
                                  obj.material.getId().getAsString(),
                                  obj.cube.getId().getAsString(),
                                  obj.mesh.getId().getAsString(),
                                  obj.sphere.getId().getAsString());
    }
};

struct NodeInsertQuery final : alex::InsertQuery<Node::descriptor_t>
{
    NodeInsertQuery() = delete;

    NodeInsertQuery(const NodeInsertQuery&) = delete;

    NodeInsertQuery(NodeInsertQuery&&) noexcept = delete;

    explicit NodeInsertQuery(type_descriptor_t desc) : InsertQuery(std::move(desc)) {}

    ~NodeInsertQuery() noexcept override = default;

    NodeInsertQuery& operator=(const NodeInsertQuery&) = delete;

    NodeInsertQuery& operator=(NodeInsertQuery&&) noexcept = delete;

    void operator()(object_t& instance) override
    {
        if (instance.name.empty()) throw std::runtime_error("Cannot insert node without name.");
        if (instance.material.isNone()) throw std::runtime_error("Cannot insert node without material attached.");
        if (instance.cube.isNone() && instance.mesh.isNone() && instance.sphere.isNone())
            throw std::runtime_error("Cannot insert node without geometry attached.");

        return InsertQuery::operator()(instance);
    }
};
