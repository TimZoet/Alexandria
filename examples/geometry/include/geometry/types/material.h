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

struct Material
{
    alex::InstanceId id;
    std::string      name;
    float3           color;
    float            specular = 0;

    using descriptor_t   = alex::GenerateTypeDescriptor<alex::Member<"id", &Material::id>,
                                                      alex::Member<"name", &Material::name>,
                                                      alex::NestedMember<"color", float3_t, &Material::color>,
                                                      alex::Member<"specular", &Material::specular>>;
    using delete_query_t = alex::DeleteQuery<descriptor_t>;
    using get_query_t    = alex::GetQuery<descriptor_t>;

    friend std::ostream& operator<<(std::ostream& out, const Material& obj)
    {
        return out << std::format("Material{{name={}, color=[{}, {}, {}], specular={}}}",
                                  obj.name,
                                  obj.color.x,
                                  obj.color.y,
                                  obj.color.x,
                                  obj.specular);
    }
};

struct MaterialInsertQuery final : alex::InsertQuery<Material::descriptor_t>
{
    MaterialInsertQuery() = delete;

    MaterialInsertQuery(const MaterialInsertQuery&) = delete;

    MaterialInsertQuery(MaterialInsertQuery&&) noexcept = delete;

    explicit MaterialInsertQuery(type_descriptor_t desc) : InsertQuery(std::move(desc)) {}

    ~MaterialInsertQuery() noexcept override = default;

    MaterialInsertQuery& operator=(const MaterialInsertQuery&) = delete;

    MaterialInsertQuery& operator=(MaterialInsertQuery&&) noexcept = delete;

    void operator()(object_t& instance) override
    {
        if (instance.name.empty()) throw std::runtime_error("Cannot insert material without name.");
        if (instance.color.x < 0 || instance.color.x > 1 || instance.color.y < 0 || instance.color.y > 1 ||
            instance.color.z < 0 || instance.color.z > 1)
            throw std::runtime_error("Cannot insert material with color outside of [0, 1] range.");
        if (instance.specular < 0 || instance.specular > 1)
            throw std::runtime_error("Cannot insert material with specular outside of [0, 1] range.");

        return InsertQuery::operator()(instance);
    }
};
