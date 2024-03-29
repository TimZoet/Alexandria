#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <vector>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria-core/member.h"
#include "alexandria-core/type_descriptor.h"
#include "alexandria-core/properties/blob.h"
#include "alexandria-core/properties/instance_id.h"
#include "alexandria-basic-query/delete_query.h"
#include "alexandria-basic-query/get_query.h"
#include "alexandria-basic-query/insert_query.h"

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

    using descriptor_t   = alex::GenerateTypeDescriptor<alex::Member<"id", &Mesh::id>,
                                                      alex::Member<"name", &Mesh::name>,
                                                      alex::Member<"vertices", &Mesh::vertices>,
                                                      alex::Member<"indices", &Mesh::indices>>;
    using delete_query_t = alex::DeleteQuery<descriptor_t>;
    using get_query_t    = alex::GetQuery<descriptor_t>;

    friend std::ostream& operator<<(std::ostream& out, const Mesh& obj)
    {
        return out << std::format("Mesh{{name={}, length(vertices)={}, length(indices)={}}}",
                                  obj.name,
                                  obj.vertices.get().size(),
                                  obj.indices.get().size());
    }
};

struct MeshInsertQuery final : alex::InsertQuery<Mesh::descriptor_t>
{
    MeshInsertQuery() = delete;

    MeshInsertQuery(const MeshInsertQuery&) = delete;

    MeshInsertQuery(MeshInsertQuery&&) noexcept = delete;

    explicit MeshInsertQuery(type_descriptor_t desc) : InsertQuery(std::move(desc)) {}

    ~MeshInsertQuery() noexcept override = default;

    MeshInsertQuery& operator=(const MeshInsertQuery&) = delete;

    MeshInsertQuery& operator=(MeshInsertQuery&&) noexcept = delete;

    void operator()(object_t& instance) override
    {
        if (instance.name.empty()) throw std::runtime_error("Cannot insert mesh without name.");

        return InsertQuery::operator()(instance);
    }
};
