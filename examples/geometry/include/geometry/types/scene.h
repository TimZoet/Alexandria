#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <string>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria/core/member.h"
#include "alexandria/core/type_descriptor.h"
#include "alexandria/member_types/instance_id.h"
#include "alexandria/member_types/reference_array.h"
#include "alexandria/queries/get_query.h"
#include "alexandria/queries/insert_query.h"

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
            objects.emplace_back(std::move(o));
        }

        return objects;
    }

    get_query_t* getQuery = nullptr;
};

struct Scene
{
    alex::InstanceId                            id;
    std::string                                 name;
    QueryableReferenceArray<Node::descriptor_t> nodes;

    using descriptor_t =
      alex::GenerateTypeDescriptor<alex::Member<&Scene::id>, alex::Member<&Scene::name>, alex::Member<&Scene::nodes>>;

    using get_query_t    = alex::GetQuery<descriptor_t>;
    using insert_query_t = alex::InsertQuery<descriptor_t>;
};
