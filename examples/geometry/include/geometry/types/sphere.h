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

struct Sphere
{
    alex::InstanceId id;
    std::string      name;
    float            radius = 0;

    using descriptor_t = alex::
      GenerateTypeDescriptor<alex::Member<&Sphere::id>, alex::Member<&Sphere::name>, alex::Member<&Sphere::radius>>;
    using delete_query_t = alex::DeleteQuery<descriptor_t>;
    using get_query_t    = alex::GetQuery<descriptor_t>;

    friend std::ostream& operator<<(std::ostream& out, const Sphere& obj)
    {
        return out << std::format("Sphere{{name={}, radius={}}}", obj.name, obj.radius);
    }
};

struct SphereInsertQuery final : alex::InsertQuery<Sphere::descriptor_t>
{
    SphereInsertQuery() = delete;

    SphereInsertQuery(const SphereInsertQuery&) = delete;

    SphereInsertQuery(SphereInsertQuery&&) noexcept = delete;

    explicit SphereInsertQuery(type_descriptor_t desc) : InsertQuery(std::move(desc)) {}

    ~SphereInsertQuery() noexcept override = default;

    SphereInsertQuery& operator=(const SphereInsertQuery&) = delete;

    SphereInsertQuery& operator=(SphereInsertQuery&&) noexcept = delete;

    void operator()(object_t& instance) override
    {
        if (instance.name.empty()) throw std::runtime_error("Cannot insert sphere without name.");
        if (instance.radius <= 0) throw std::runtime_error("Cannot insert sphere with radius <= 0.");

        return InsertQuery::operator()(instance);
    }
};
