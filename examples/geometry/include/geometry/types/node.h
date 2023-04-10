#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <format>
#include <iostream>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria-core/member.h"
#include "alexandria-core/type_descriptor.h"
#include "alexandria-core/properties/instance_id.h"
#include "alexandria-core/properties/reference.h"
#include "alexandria-basic-query/delete_query.h"
#include "alexandria-basic-query/get_query.h"
#include "alexandria-basic-query/insert_query.h"

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

    using descriptor_t   = alex::GenerateTypeDescriptor<alex::Member<"id", &Node::id>,
                                                      alex::Member<"name", &Node::name>,
                                                      alex::NestedMember<"translation", float3_t, &Node::translation>,
                                                      alex::Member<"material", &Node::material>,
                                                      alex::Member<"cube", &Node::cube>,
                                                      alex::Member<"mesh", &Node::mesh>,
                                                      alex::Member<"sphere", &Node::sphere>>;
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
