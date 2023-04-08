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

        auto& operator()(Ps&&... params)
        {
            [&]<size_t... Is>(std::index_sequence<Is...>, auto&& tuple)
            {
                return ((*std::get<Is>(parameters) = std::get<Is>(tuple)), ...);
            }
            (std::make_index_sequence<sizeof...(Ps)>{}, std::make_tuple<Ps...>(std::forward<Ps>(params)...));
            statement.bind(sql::BindParameters::All);

            return *this;
        }

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        type_descriptor_t descriptor;
        statement_t       statement;
        parameters_t      parameters;
    };
}  // namespace alex
