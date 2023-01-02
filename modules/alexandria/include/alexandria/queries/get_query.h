#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <memory>
#include <tuple>
#include <vector>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "cppql-core/database.h"
#include "cppql-typed/queries/select.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "alexandria/member_types/instance_id.h"

namespace alex
{
    template<typename T, typename F>
    class Query
    {
    public:
        using object_handler_t = T;
        using object_t         = typename object_handler_t::object_t;
        using select_t = sql::select_t<typename object_handler_t::prim_handler_t::table_t, InstanceId, 0>;

        Query(T& objectHandler, F filter) : select()
        {
            auto& table = objectHandler.getPrimitiveHandler().getTable();
            select      = table.template select<InstanceId, 0>(std::move(filter), true);
        }

        std::vector<InstanceId> operator()() { return std::vector<InstanceId>(select(true).begin(), select.end()); }

    private:
        select_t select;
    };
}  // namespace alex