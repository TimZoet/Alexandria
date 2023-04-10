#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <functional>
#include <tuple>
#include <type_traits>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "common/static_assert.h"
#include "alexandria-core/type_descriptor.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "alexandria-extended-query/search_query.h"
#include "alexandria-extended-query/table_sets.h"

namespace alex
{
    namespace detail
    {
        template<typename T, MemberName M>
        struct ReferenceSearchOperator
        {
            using descriptor_t = T;
            static constexpr auto name() { return M; }
        };

        template<bool Intersect, typename U>
        struct UnionWrapper
        {
            U u;
        };

        template<bool Intersect, typename U, typename J>
        [[nodiscard]] auto operator+(UnionWrapper<Intersect, U>&& wrapper, J&& join)
        {
            using U2 = decltype(wrapper.u.unions(sql::UnionOperator::Intersect, std::forward<J>(join)));
            return UnionWrapper<Intersect, U2>{wrapper.u.unions(
              Intersect ? sql::UnionOperator::Intersect : sql::UnionOperator::Union, std::forward<J>(join))};
        }

        // TODO: Constrain operators to ReferenceSearchOperator for reference array properties of T.
        template<bool And, typename T>
        [[nodiscard]] auto referenceSearchImpl(T& tables, auto... operators)
        {
            auto& instTable = tables.getInstanceTable();
            auto  ops       = std::make_tuple(operators...);

            // Construct the list of parameters as pointers to allow dynamic binding.
            auto params =
              std::make_tuple(std::make_unique<std::conditional_t<true, std::string, decltype(operators)>>()...);

            // For each reference array table, create a join between it and the instance table.
            // Join on the uuid column of the instance table and the instance column of the reference array table.
            // Then filter by the value column of the reference array table.
            auto joins = [&]<size_t... Is>(std::index_sequence<Is...>)
            {
                return std::make_tuple(
                  instTable
                    .join(sql::InnerJoin,
                          tables.getReferenceArrayTable<std::decay_t<decltype(std::get<Is>(ops))>::name()>())
                    .on(tables.template getInstanceColumn<"id">() ==
                          tables.getReferenceArrayTable<std::decay_t<decltype(std::get<Is>(ops))>::name()>().col<1>() &&
                        sql::like(
                          tables.getReferenceArrayTable<std::decay_t<decltype(std::get<Is>(ops))>::name()>().col<2>(),
                          std::get<Is>(params).get()))
                    .selectAs<InstanceId>(tables.template getInstanceColumn<"id">())
                    .groupBy(instTable.template col<0>())...);
            }
            (std::make_index_sequence<sizeof...(operators)>{});

            // Create union of all selects.
            auto query = [&]<size_t I, size_t... Is>(std::index_sequence<I, Is...>)
            {
                return (UnionWrapper<And, std::decay_t<decltype(std::get<I>(joins))>>{.u = std::get<I>(joins)} + ... +
                        std::get<Is>(joins))
                  .u;
            }
            (std::make_index_sequence<sizeof...(operators)>{});

            auto stmt = query.compile();
            return SearchQuery(tables.getTypeDescriptor(), std::move(stmt), std::move(params));
        }
    }  // namespace detail

    // TODO: Constrain M to reference array.

    template<is_type_descriptor T, detail::MemberName M>
        requires(detail::is_reference_array_member_name<M, T>)
    [[nodiscard]] auto references()
    {
        return detail::ReferenceSearchOperator<T, M>{};
    }

    // TODO: Constrain T to tablesets.

    /**
     * \brief Construct a SearchQuery to find all instances for which the primitive search operator is true.
     * \tparam T TableSets type.
     * \param tables TableSets instance.
     * \param op Single PrimitiveSearchOperator.
     * \return SearchQuery.
     */
    template<typename T>
    [[nodiscard]] auto referenceSearch(T& tables, auto op)
    {
        return detail::referenceSearchImpl<true>(tables, std::move(op));
    }

    /**
     * \brief Construct a SearchQuery to find all instances for which the conjunction (&&) of primitive search operators is true.
     * \tparam T TableSets type.
     * \param tables TableSets instance.
     * \param op Single PrimitiveSearchOperator.
     * \param operators PrimitiveSearchOperators.
     * \return SearchQuery.
     */
    template<typename T>
    [[nodiscard]] auto referenceSearchAnd(T& tables, auto op, auto... operators)
    {
        return detail::referenceSearchImpl<true>(tables, std::move(op), std::move(operators)...);
    }

    /**
     * \brief Construct a SearchQuery to find all instances for which the disjunction (||) of primitive search operators is true.
     * \tparam T TableSets type.
     * \param tables TableSets instance.
     * \param op Single PrimitiveSearchOperator.
     * \param operators PrimitiveSearchOperators.
     * \return SearchQuery.
     */
    template<typename T>
    [[nodiscard]] auto referenceSearchOr(T& tables, auto op, auto... operators)
    {
        return detail::referenceSearchImpl<false>(tables, std::move(op), std::move(operators)...);
    }

    /**
     * \brief Construct a SearchQuery to find all instances for which the primitive search operator is true.
     * \tparam T TypeDescriptor type.
     * \param desc TypeDescriptor instance.
     * \param op Single PrimitiveSearchOperator.
     * \return SearchQuery.
     */
    template<is_type_descriptor T>
    [[nodiscard]] auto referenceSearch(T desc, auto op)
    {
        auto tables = TableSets(desc);
        return referenceSearch(tables, std::move(op));
    }

    /**
     * \brief Construct a SearchQuery to find all instances for which the conjunction (&&) of primitive search operators is true.
     * \tparam T TypeDescriptor type.
     * \param desc TypeDescriptor instance.
     * \param op Single PrimitiveSearchOperator.
     * \param operators PrimitiveSearchOperators.
     * \return SearchQuery.
     */
    template<is_type_descriptor T>
    [[nodiscard]] auto referenceSearchAnd(T desc, auto op, auto... operators)
    {
        auto tables = TableSets(desc);
        return referenceSearchAnd(tables, std::move(op), std::move(operators)...);
    }

    /**
     * \brief Construct a SearchQuery to find all instances for which the disjunction (||) of primitive search operators is true.
     * \tparam T TypeDescriptor type.
     * \param desc TypeDescriptor instance.
     * \param op Single PrimitiveSearchOperator.
     * \param operators PrimitiveSearchOperators.
     * \return SearchQuery.
     */
    template<is_type_descriptor T>
    [[nodiscard]] auto referenceSearchOr(T desc, auto op, auto... operators)
    {
        auto tables = TableSets(desc);
        return referenceSearchOr(tables, std::move(op), std::move(operators)...);
    }

}  // namespace alex
