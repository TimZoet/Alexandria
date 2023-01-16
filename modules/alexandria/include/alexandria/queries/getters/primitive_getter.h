#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <type_traits>

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "alexandria/queries/utils.h"
#include "alexandria/queries/types/member_extractor.h"

namespace alex
{
    /**
     * \brief The PrimitiveGetter handles the retrieval of all columns of the instance table. This includes not
     * just integers and floats, but also the UUID and single string, blob and reference columns.
     * \tparam T TypeDescriptor.
     */
    template<typename T>
    class PrimitiveGetter
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief TypeDescriptor.
         */
        using type_descriptor_t = T;

        /**
         * \brief Object type.
         */
        using object_t = typename type_descriptor_t::object_t;

        /**
         * \brief Concatenation of the UUID member and all primitive members.
         */
        using members_t = detail::extract_primitive_members_t<typename type_descriptor_t::members_t>;

        /**
         * \brief TypedTable for the instance table.
         */
        using table_t = detail::primitive_table_t<members_t>;

        /**
         * \brief Select query type.
         */
        using query_t = std::remove_cvref_t<decltype(std::declval<table_t>().select())>;

        /**
         * \brief Select statement type.
         */
        using statement_t = std::remove_cvref_t<decltype(std::declval<query_t>().compileOne())>;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        PrimitiveGetter() = delete;

        PrimitiveGetter(const type_descriptor_t& desc, std::string& uuidParam) : statement(compile(desc, uuidParam)) {}

        PrimitiveGetter(const PrimitiveGetter&) = delete;

        PrimitiveGetter(PrimitiveGetter&&) = default;

        ~PrimitiveGetter() noexcept = default;

        PrimitiveGetter& operator=(const PrimitiveGetter&) = delete;

        PrimitiveGetter& operator=(PrimitiveGetter&&) = default;

        ////////////////////////////////////////////////////////////////
        // Invoke.
        ////////////////////////////////////////////////////////////////

        void operator()(object_t& instance)
        {
            const auto setter = [&instance]<typename M, typename V>(M, V&& val) {
                if constexpr (M::is_instance_id || M::is_primitive || M::is_string || M::is_reference)
                    M::template get(instance) = std::forward<V>(val);
                else if constexpr (M::is_primitive_blob || M::is_blob)
                    M::template get(instance).set(std::forward<V>(val));
                else
                    constexpr_static_assert();
            };

            const auto g = [&]<size_t... Is>(std::index_sequence<Is...>)
            {
                // Run statement to retrieve tuple. Includes rowid, which we want to skip when writing to the instance. Hence the std::get<Is+1>.
                auto tuple = statement.bind(sql::BindParameters::Dynamic)();
                // Move each value into the instance.
                (setter(std::tuple_element_t<Is, members_t>{}, std::move(std::get<Is + 1>(tuple))), ...);
            };

            const auto f = [&]<typename... Ms>(std::tuple<Ms...>) { g(std::index_sequence_for<Ms...>{}); };

            f(members_t{});

            statement.clearBindings();
        }

    private:
        [[nodiscard]] static statement_t compile(const type_descriptor_t& desc, std::string& uuidParam)
        {
            const auto table = table_t(desc.getType().getInstanceTable());
            return table.select().where(sql::like(table.template col<1>(), &uuidParam)).compileOne();
        }

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        statement_t statement;
    };
}  // namespace alex
