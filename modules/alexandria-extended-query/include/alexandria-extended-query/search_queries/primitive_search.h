#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

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
        enum class PrimitiveSearchOp
        {
            Equal        = 0,
            NotEqual     = 1,
            Less         = 2,
            Greater      = 3,
            LessEqual    = 4,
            GreaterEqual = 5,
            None         = 6
        };

        template<typename T, MemberName M, PrimitiveSearchOp O>
        struct PrimitiveSearchOperator
        {
            using descriptor_t = T;
            static constexpr auto name() { return M; }
            static constexpr auto op() { return O; }
        };

        // TODO: Constrain operators to PrimitiveSearchOperators for primitive properties of T.
        template<bool And, typename T>
        [[nodiscard]] auto primitiveSearchImpl(T& tables, auto... operators)
        {
            auto& instTable = tables.getInstanceTable();
            auto  cols =
              std::make_tuple(tables.template getInstanceColumn<std::decay_t<decltype(operators)>::name()>()...);
            auto ops = std::make_tuple(operators...);

            // Construct the list of parameters as pointers to allow dynamic binding.
            auto params =
              std::make_tuple(std::make_unique<decltype(tables.template getInstanceColumn<
                                                        std::decay_t<decltype(operators)>::name()>())::value_t>()...);

            constexpr auto op = []<typename O>(O) {
                if constexpr (O::op() == PrimitiveSearchOp::Equal)
                    return [](const auto& col, const auto& param) {
                        if constexpr (std::same_as<std::string, std::remove_pointer_t<std::decay_t<decltype(param)>>>)
                            return sql::like(col, param);
                        else
                            return col == param;
                    };
                else if constexpr (O::op() == PrimitiveSearchOp::NotEqual)
                    return [](const auto& col, const auto& param) { return col != param; };
                else if constexpr (O::op() == PrimitiveSearchOp::Less)
                    return [](const auto& col, const auto& param) { return col < param; };
                else if constexpr (O::op() == PrimitiveSearchOp::Greater)
                    return [](const auto& col, const auto& param) { return col > param; };
                else if constexpr (O::op() == PrimitiveSearchOp::LessEqual)
                    return [](const auto& col, const auto& param) { return col <= param; };
                else if constexpr (O::op() == PrimitiveSearchOp::GreaterEqual)
                    return [](const auto& col, const auto& param) { return col >= param; };
                else if constexpr (O::op() == PrimitiveSearchOp::None)
                    return [](const auto& col, const auto&) { return col == nullptr; };
                else
                    constexpr_static_assert<false>();
            };

            // Construct an expression with all column-parameter pairs of the form:
            // col[0] op[0] param[0] &&/|| ... &&/|| col[N] op[N] param[N]
            auto expr = [&]<size_t... Is>(std::index_sequence<Is...>)
            {
                if constexpr (And)
                    return (op(std::get<Is>(ops))(std::get<Is>(cols), std::get<Is>(params).get()) && ...);
                else
                    return (op(std::get<Is>(ops))(std::get<Is>(cols), std::get<Is>(params).get()) || ...);
            }
            (std::make_index_sequence<sizeof...(operators)>{});

            // Compile statement that selects instance identifiers.
            auto stmt = instTable.template selectAs<InstanceId>(tables.template getInstanceColumn<"id">())
                          .where(std::move(expr))
                          .orderBy(sql::ascending(instTable.template col<0>()))
                          .compile();

            return SearchQuery(tables.getTypeDescriptor(), std::move(stmt), std::move(params));
        }
    }  // namespace detail

    /**
     * \brief Compare a primitive property using the == operator.
     * \tparam T TypeDescriptor.
     * \tparam M MemberName.
     * \return PrimitiveSearchOperator which can be passed to the primitiveSearch* functions.
     */
    template<is_type_descriptor T, detail::MemberName M>
        requires(detail::is_primitive_member_name<M, T>)
    [[nodiscard]] auto equal()
    {
        return detail::PrimitiveSearchOperator<T, M, detail::PrimitiveSearchOp::Equal>{};
    }

    /**
     * \brief Compare a primitive property using the != operator.
     * \tparam T TypeDescriptor.
     * \tparam M MemberName.
     * \return PrimitiveSearchOperator which can be passed to the primitiveSearch* functions.
     */
    template<is_type_descriptor T, detail::MemberName M>
        requires(detail::is_primitive_member_name<M, T>)
    [[nodiscard]] auto notEqual()
    {
        return detail::PrimitiveSearchOperator<T, M, detail::PrimitiveSearchOp::NotEqual>{};
    }

    /**
     * \brief Compare a primitive property using the < operator.
     * \tparam T TypeDescriptor.
     * \tparam M MemberName.
     * \return PrimitiveSearchOperator which can be passed to the primitiveSearch* functions.
     */
    template<is_type_descriptor T, detail::MemberName M>
        requires(detail::is_primitive_member_name<M, T>)
    [[nodiscard]] auto less()
    {
        return detail::PrimitiveSearchOperator<T, M, detail::PrimitiveSearchOp::Less>{};
    }

    /**
     * \brief Compare a primitive property using the > operator.
     * \tparam T TypeDescriptor.
     * \tparam M MemberName.
     * \return PrimitiveSearchOperator which can be passed to the primitiveSearch* functions.
     */
    template<is_type_descriptor T, detail::MemberName M>
        requires(detail::is_primitive_member_name<M, T>)
    [[nodiscard]] auto greater()
    {
        return detail::PrimitiveSearchOperator<T, M, detail::PrimitiveSearchOp::Greater>{};
    }

    /**
     * \brief Compare a primitive property using the <= operator.
     * \tparam T TypeDescriptor.
     * \tparam M MemberName.
     * \return PrimitiveSearchOperator which can be passed to the primitiveSearch* functions.
     */
    template<is_type_descriptor T, detail::MemberName M>
        requires(detail::is_primitive_member_name<M, T>)
    [[nodiscard]] auto lessEqual()
    {
        return detail::PrimitiveSearchOperator<T, M, detail::PrimitiveSearchOp::LessEqual>{};
    }

    /**
     * \brief Compare a primitive property using the >= operator.
     * \tparam T TypeDescriptor.
     * \tparam M MemberName.
     * \return PrimitiveSearchOperator which can be passed to the primitiveSearch* functions.
     */
    template<is_type_descriptor T, detail::MemberName M>
        requires(detail::is_primitive_member_name<M, T>)
    [[nodiscard]] auto greaterEqual()
    {
        return detail::PrimitiveSearchOperator<T, M, detail::PrimitiveSearchOp::GreaterEqual>{};
    }

    // TODO: Constrain M to ref prop.
    /**
     * \brief Check if a reference property is none/null/empty.
     * \tparam T TypeDescriptor.
     * \tparam M MemberName.
     * \return PrimitiveSearchOperator which can be passed to the primitiveSearch* functions.
     */
    template<is_type_descriptor T, detail::MemberName M>
    [[nodiscard]] auto none()
    {
        return detail::PrimitiveSearchOperator<T, M, detail::PrimitiveSearchOp::None>{};
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
    [[nodiscard]] auto primitiveSearch(T& tables, auto op)
    {
        return detail::primitiveSearchImpl<true>(tables, std::move(op));
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
    [[nodiscard]] auto primitiveSearchAnd(T& tables, auto op, auto... operators)
    {
        return detail::primitiveSearchImpl<true>(tables, std::move(op), std::move(operators)...);
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
    [[nodiscard]] auto primitiveSearchOr(T& tables, auto op, auto... operators)
    {
        return detail::primitiveSearchImpl<false>(tables, std::move(op), std::move(operators)...);
    }

    /**
     * \brief Construct a SearchQuery to find all instances for which the primitive search operator is true.
     * \tparam T TypeDescriptor type.
     * \param desc TypeDescriptor instance.
     * \param op Single PrimitiveSearchOperator.
     * \return SearchQuery.
     */
    template<is_type_descriptor T>
    [[nodiscard]] auto primitiveSearch(T desc, auto op)
    {
        auto tables = TableSets(desc);
        return primitiveSearch(tables, std::move(op));
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
    [[nodiscard]] auto primitiveSearchAnd(T desc, auto op, auto... operators)
    {
        auto tables = TableSets(desc);
        return primitiveSearchAnd(tables, std::move(op), std::move(operators)...);
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
    [[nodiscard]] auto primitiveSearchOr(T desc, auto op, auto... operators)
    {
        auto tables = TableSets(desc);
        return primitiveSearchOr(tables, std::move(op), std::move(operators)...);
    }
}  // namespace alex
