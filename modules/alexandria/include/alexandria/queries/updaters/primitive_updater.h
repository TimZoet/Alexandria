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
    namespace detail
    {
        template<typename T, size_t... Is>
        struct UpdateStatementGeneratorImpl
        {
            using type = decltype(std::declval<T>().template update<Is...>());
        };

        template<typename...>
        struct UpdateStatementGenerator
        {
        };

        template<typename C, typename... Cs>
        struct UpdateStatementGenerator<sql::TypedTable<C, Cs...>>
        {
            using table_t              = sql::TypedTable<C, Cs...>;
            static constexpr auto func = []<size_t I, size_t... Is>(std::index_sequence<I, Is...>)
                                           -> decltype(std::declval<table_t>().template update<Is...>())
            {
                return std::declval<table_t>().template update<Is...>();
            };

            using type = std::remove_cvref_t<std::invoke_result_t<decltype(func), std::index_sequence_for<C, Cs...>>>;
        };
    }  // namespace detail

    /**
     * \brief The PrimitiveUpdater handles the updating of all columns of the instance table. This includes not
     * just integers and floats, but also the UUID and single string, blob and reference columns.
     * \tparam T TypeDescriptor.
     */
    template<typename T>
    class PrimitiveUpdater
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
         * \brief Update query type.
         */
        using query_t = typename detail::UpdateStatementGenerator<
          table_t>::type;  //std::remove_cvref_t<decltype(std::declval<table_t>().update())>;

        /**
         * \brief Update statement type.
         */
        using statement_t = std::remove_cvref_t<decltype(std::declval<query_t>().compile())>;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        PrimitiveUpdater() = delete;

        PrimitiveUpdater(const type_descriptor_t& desc, std::string& uuidParam) : statement(compile(desc, uuidParam)) {}

        PrimitiveUpdater(const PrimitiveUpdater&) = delete;

        PrimitiveUpdater(PrimitiveUpdater&&) = default;

        ~PrimitiveUpdater() noexcept = default;

        PrimitiveUpdater& operator=(const PrimitiveUpdater&) = delete;

        PrimitiveUpdater& operator=(PrimitiveUpdater&&) = default;

        ////////////////////////////////////////////////////////////////
        // Invoke.
        ////////////////////////////////////////////////////////////////

        void operator()(object_t& instance)
        {
            statement.bind(sql::BindParameters::Dynamic);

            // TODO: This getter needs to be moved out of here.
            const auto getter = [&]<typename M>(M) {
                // TODO: All these toText copy data. Might not always be necessary.
                if constexpr (M::is_instance_id)
                    return sql::toText(M::template get(instance).getAsString());
                else if constexpr (M::is_primitive_blob || M::is_blob)
                {
                    if constexpr (std::convertible_to<decltype(M::template get(instance)), sql::StaticBlob>)
                        return static_cast<sql::StaticBlob>(M::template get(instance));
                    else if constexpr (std::convertible_to<decltype(M::template get(instance)), sql::TransientBlob>)
                        return static_cast<sql::TransientBlob>(M::template get(instance));
                    else
                        return static_cast<sql::Blob>(M::template get(instance));
                }
                else if constexpr (M::is_reference)
                    // TODO: What if no object was assigned? Turn return type into std::optional and rely on cppql to insert nullptr?
                    return sql::toText(M::template get(instance).getId().getAsString());
                else if constexpr (M::is_primitive)
                    return M::template get(instance);
                else if constexpr (M::is_string)
                    return sql::toText(M::template get(instance));
                else
                    constexpr_static_assert();
            };

            const auto f = [&]<typename... Ms>(std::tuple<Ms...>) {
                // Retrieve member values from instance for each column.
                statement(getter(Ms())...);
            };

            f(members_t{});

            statement.clearBindings();
        }

    private:
        [[nodiscard]] static statement_t compile(const type_descriptor_t& desc, std::string& uuidParam)
        {
            const auto table = table_t(desc.getType().getInstanceTable());

            const auto f = [&]<size_t I, size_t... Is>(std::index_sequence<I, Is...>)
            {
                return table.template update<Is...>().where(sql::like(table.template col<1>(), &uuidParam)).compile();
            };

            const auto g = [&]<typename C, typename... Cs>(const sql::TypedTable<C, Cs...>&) {
                return f(std::index_sequence_for<C, Cs...>{});
            };

            return g(table);
            //return f(std::index_sequence_for<>)
            //return table.update().where(sql::like(table.template col<1>(), &uuidParam)).compile();
        }

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        statement_t statement;
    };
}  // namespace alex
