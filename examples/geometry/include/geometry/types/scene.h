#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <format>
#include <iostream>
#include <string>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria-core/member.h"
#include "alexandria-core/type_descriptor.h"
#include "alexandria-core/properties/instance_id.h"
#include "alexandria-core/properties/reference_array.h"
#include "alexandria-query/delete_query.h"
#include "alexandria-query/get_query.h"
#include "alexandria-query/insert_query.h"
#include "alexandria-query/update_query.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "geometry/types/node.h"

// TODO: Move into separate file.
template<typename T>
struct QueryableReferenceArray : alex::ReferenceArray<typename T::object_t>
{
    using descriptor_t = T;
    using object_t     = typename T::object_t;
    using get_query_t  = alex::GetQuery<descriptor_t>;

    [[nodiscard]] std::vector<object_t> getAll() const
    {
        if (!getQuery) throw std::runtime_error("");

        std::vector<object_t> objects;

        for (const auto& id : this->get())
        {
            object_t o{.id = id};
            (*getQuery)(o);
            objects.emplace_back(std::move(o));
        }

        return objects;
    }

    get_query_t* getQuery = nullptr;
};

template<typename T>
struct alex::_is_reference_array<QueryableReferenceArray<T>> : std::true_type
{
};

struct Scene
{
    alex::InstanceId                            id;
    std::string                                 name;
    QueryableReferenceArray<Node::descriptor_t> nodes;

    using descriptor_t = alex::GenerateTypeDescriptor<alex::Member<"id", &Scene::id>,
                                                      alex::Member<"name", &Scene::name>,
                                                      alex::Member<"nodes", &Scene::nodes>>;

    using delete_query_t = alex::DeleteQuery<descriptor_t>;
    using get_query_t    = alex::GetQuery<descriptor_t>;
    using update_query_t = alex::UpdateQuery<descriptor_t>;

    friend std::ostream& operator<<(std::ostream& out, const Scene& obj)
    {
        return out << std::format("Scene{{name={}, length(nodes)={}}}", obj.name, obj.nodes.get().size());
    }
};

struct SceneInsertQuery final : alex::InsertQuery<Scene::descriptor_t>
{
    SceneInsertQuery() = delete;

    SceneInsertQuery(const SceneInsertQuery&) = delete;

    SceneInsertQuery(SceneInsertQuery&&) noexcept = delete;

    explicit SceneInsertQuery(type_descriptor_t desc) : InsertQuery(std::move(desc)) {}

    ~SceneInsertQuery() noexcept override = default;

    SceneInsertQuery& operator=(const SceneInsertQuery&) = delete;

    SceneInsertQuery& operator=(SceneInsertQuery&&) noexcept = delete;

    void operator()(object_t& instance) override
    {
        if (instance.name.empty()) throw std::runtime_error("Cannot insert scene without name.");

        return InsertQuery::operator()(instance);
    }
};
