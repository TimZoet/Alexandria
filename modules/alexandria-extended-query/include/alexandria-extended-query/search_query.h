#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <tuple>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "cppql/statements/select_statement.h"

namespace alex
{
    /**
     * \brief SearchQuery.
     * \tparam T TypeDescriptor.
     * \tparam S sql::SelectStatement.
     * \tparam Ps Parameters.
     */
    template<typename T, typename S, typename... Ps>
    class SearchQuery
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        using type_descriptor_t = T;
        using object_t          = typename type_descriptor_t::object_t;
        using statement_t       = S;
        using parameters_t      = std::tuple<std::unique_ptr<Ps>...>;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        SearchQuery() = delete;

        SearchQuery(type_descriptor_t desc, statement_t stmt, parameters_t params) :
            descriptor(desc), statement(std::move(stmt)), parameters(std::move(params))
        {
        }

        SearchQuery(const SearchQuery&) = delete;

        SearchQuery(SearchQuery&& other) noexcept = default;

        ~SearchQuery() noexcept = default;

        SearchQuery& operator=(const SearchQuery&) = delete;

        SearchQuery& operator=(SearchQuery&& other) noexcept = default;

        ////////////////////////////////////////////////////////////////
        // ...
        ////////////////////////////////////////////////////////////////

        auto begin() { return statement.begin(); }

        auto end() { return statement.end(); }

        template<typename... Ts>
            requires(sizeof...(Ts) == sizeof...(Ps))
        auto& operator()(Ts&&... params)
        {
            // InstanceId needs to be explicitly turned into a string.
            // Other parameters can be forwarded as-is.
            constexpr auto get = []<typename P>(P&& param) {
                if constexpr (std::same_as<InstanceId, std::decay_t<P>>)
                    return param.getAsString();
                else
                    return std::forward<P>(param);
            };

            bind<0>(get(std::forward<Ts>(params))...);
            statement.bind(sql::BindParameters::Dynamic);
            return *this;
        }

    private:
        template<size_t I, typename Param, typename... Params>
        void bind(Param&& param, Params&&... params)
        {
            bind<I>(std::forward<Param>(param));
            bind<I + 1>(std::forward<Params>(params)...);
        }

        template<size_t I, typename Param>
        void bind(Param&& param)
        {
            using type = typename std::tuple_element_t<I, parameters_t>::element_type;
            if constexpr (!std::same_as<std::nullptr_t, std::decay_t<Param>>)
                *std::get<I>(parameters) = static_cast<type>(param);
        }

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        type_descriptor_t descriptor;
        statement_t       statement;
        parameters_t      parameters;
    };
}  // namespace alex
