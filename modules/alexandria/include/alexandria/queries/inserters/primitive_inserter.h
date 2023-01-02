#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "alexandria/queries/utils.h"

namespace alex
{
    template<typename T>
    class PrimitiveInserter
    {
    public:
        using type_descriptor_t = T;
        using object_t          = typename type_descriptor_t::object_t;
        using primitive_members_t =
          tuple_merge_t<std::tuple<typename type_descriptor_t::uuid_member_t>,
                        detail::extract_primitive_members_t<typename type_descriptor_t::user_members_t>>;
        using table_t     = detail::primitive_table_t<primitive_members_t>;
        using query_t     = std::remove_cvref_t<decltype(std::declval<table_t>().insert())>;
        using statement_t = std::remove_cvref_t<decltype(std::declval<query_t>().compile())>;

        explicit PrimitiveInserter(type_descriptor_t desc) : descriptor(std::move(desc)) {}

        void operator()(object_t& instance)
        {
            initialize();

            const auto getter = [&]<typename M>(M) {
                if constexpr (M::is_instance_id)
                    return sql::toText(M::template get(instance).getAsString());
                else if constexpr (M::is_blob || M::is_primitive_blob)
                    return M::template get(instance).getStaticBlob();
                else if constexpr (M::is_reference)
                    // TODO: What if no object was assigned? Turn return type into std::optional and rely on cppql to insert nullptr?
                    return sql::toText(M::template get(instance).getAsString());
                else if constexpr (M::is_primitive)
                    return M::template get(instance);
                else if constexpr (M::is_string)
                    return sql::toText(M::template get(instance));
                else
                    constexpr_static_assert();
            };

            const auto f = [&]<typename... Ms>(std::tuple<Ms...>) { (*statement)(getter(Ms{})...); };

            f(primitive_members_t{});
        }

    private:
        ////////////////////////////////////////////////////////////////
        // Private methods.
        ////////////////////////////////////////////////////////////////

        void initialize()
        {
            if (table) return;

            table     = table_t(descriptor.getType().getInstanceTable());
            statement = table->insert().compile();
        }

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        type_descriptor_t          descriptor;
        std::optional<table_t>     table;
        std::optional<statement_t> statement;
    };
}  // namespace alex
