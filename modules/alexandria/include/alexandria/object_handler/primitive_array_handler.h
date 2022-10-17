#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <tuple>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "common/type_traits.h"
#include "cppql-core/database.h"
#include "cppql-typed/queries/delete.h"
#include "cppql-typed/queries/insert.h"
#include "cppql-typed/queries/select.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "alexandria/property.h"
#include "alexandria/type.h"
#include "alexandria/member_types/member.h"
#include "alexandria/object_handler/type_traits.h"

namespace alex
{
    ////////////////////////////////////////////////////////////////
    // PrimitiveArrayHandler type traits.
    ////////////////////////////////////////////////////////////////

    /**
     * \brief Generates a tuple of TypedTables. For each member a TypedTable<row ID, instance ID, primitive> is added to the tuple.
     * \tparam Ms Members.
     */
    template<typename... Ms>
    using primitive_array_table_t =
      std::tuple<sql::TypedTable<sql::row_id, InstanceId::type, type_to_column_t<typename Ms::value_t>>...>;

    template<typename...>
    struct primitive_array_insert
    {
        // No PrimitiveArray members resolves to void*.
        using type = void*;
    };

    template<template<typename, typename...> class Tuple, typename... Ts>
    struct primitive_array_insert<Tuple<Ts...>>
    {
        using type = std::tuple<sql::insert_t<Ts>...>;
    };

    /**
     * \brief Generates a tuple of Insert types. For each table an Insert is added to the tuple.
     * \tparam T Tuple of TypedTables.
     */
    template<typename T>
    using primitive_array_insert_t = typename primitive_array_insert<T>::type;

    template<typename...>
    struct primitive_array_select
    {
        // No PrimitiveArray members resolves to void*.
        using type = void*;
    };

    template<template<typename, typename...> class Tuple, typename... Ts>
    struct primitive_array_select<Tuple<Ts...>>
    {
        // For each TypedTable, select the column containing the primitive value.
        using type = std::tuple<sql::select_t<Ts, sql::col_t<2, Ts>, 2>...>;
    };

    /**
     * \brief Generates a tuple of Select types. For each table a Select is added to the tuple.
     * \tparam T Tuple of TypedTables.
     */
    template<typename T>
    using primitive_array_select_t = typename primitive_array_select<T>::type;

    template<typename...>
    struct primitive_array_delete
    {
        // No PrimitiveArray members resolves to void*.
        using type = void*;
    };

    template<template<typename, typename...> class Tuple, typename... Ts>
    struct primitive_array_delete<Tuple<Ts...>>
    {
        // For each TypedTable, delete matching rows.
        using type = std::tuple<sql::delete_t<Ts>...>;
    };

    /**
     * \brief Generates a tuple of Delete types. For each table a Delete is added to the tuple.
     * \tparam T Tuple of TypedTables.
     */
    template<typename T>
    using primitive_array_delete_t = typename primitive_array_delete<T>::type;

    ////////////////////////////////////////////////////////////////
    // PrimitiveArrayHandler class.
    ////////////////////////////////////////////////////////////////

    template<typename IdMember, typename... Members>
    class PrimitiveArrayHandler
    {
    public:
        using object_t      = typename IdMember::class_t;
        using table_t       = primitive_array_table_t<Members...>;
        using insert_stmt_t = primitive_array_insert_t<table_t>;
        using select_stmt_t = primitive_array_select_t<table_t>;
        using delete_stmt_t = primitive_array_delete_t<table_t>;

        PrimitiveArrayHandler() = delete;

        PrimitiveArrayHandler(const Type& t, sql::Database& db);

        PrimitiveArrayHandler(const PrimitiveArrayHandler&) = delete;

        PrimitiveArrayHandler(PrimitiveArrayHandler&&) = delete;

        PrimitiveArrayHandler& operator=(const PrimitiveArrayHandler&) = delete;

        PrimitiveArrayHandler& operator=(PrimitiveArrayHandler&&) = delete;

        ~PrimitiveArrayHandler() noexcept = default;

        ////////////////////////////////////////////////////////////////
        // Main public methods.
        ////////////////////////////////////////////////////////////////

        void insert(const object_t& instance);

        void update(const object_t& instance);

        void get(InstanceId id, object_t& instance);

        void del(InstanceId id);

        ////////////////////////////////////////////////////////////////
        // Private utility methods.
        ////////////////////////////////////////////////////////////////

    private:
        template<size_t I, typename M, typename... Ms>
        void initialize(const std::vector<std::string>& names);

        template<size_t I, typename M, typename... Ms>
        void insertImpl(const object_t& instance);

        template<size_t I, typename M, typename... Ms>
        void getImpl(object_t& instance);

        template<size_t I, typename M, typename... Ms>
        void delImpl();

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        const Type*      type;
        sql::Database*   database;
        table_t          tables;
        insert_stmt_t    insertStmt;
        select_stmt_t    selectStmt;
        InstanceId::type selectStmtIdParam = 0;
        delete_stmt_t    deleteStmt;
        InstanceId::type deleteStmtIdParam = 0;
    };

    ////////////////////////////////////////////////////////////////
    // Implementation.
    ////////////////////////////////////////////////////////////////

    template<typename IdMember, typename... Members>
    PrimitiveArrayHandler<IdMember, Members...>::PrimitiveArrayHandler(const Type& t, sql::Database& db) :
        type(&t), database(&db), tables(), insertStmt(), selectStmt(), deleteStmt()
    {
        if constexpr (sizeof...(Members) > 0)
        {
            // Get all primitive array props.
            const auto paProps = type->getProperties(
              [](const Property& prop) -> bool { return prop.isPrimitive() && prop.isReference() && prop.isArray(); });

            // Get table names to initialize all typed tables corresponding to each prop.
            std::vector<std::string> names;
            names.reserve(sizeof...(Members));
            for (const auto& prop : paProps)
            {
                const auto parents = type->getPropertyParents(*prop);
                names.push_back(type->resolveReferenceTableName(parents, *prop));
            }

            if (sizeof...(Members) != names.size())
                throw std::runtime_error("Number of static members does not match number of properties in database");

            // Initialize.
            initialize<0, Members...>(names);
        }
    }

    ////////////////////////////////////////////////////////////////
    // Main public methods.
    ////////////////////////////////////////////////////////////////

    template<typename IdMember, typename... Members>
    void PrimitiveArrayHandler<IdMember, Members...>::insert(const object_t& instance)
    {
        // Call recursive implementation method if there are members.
        if constexpr (sizeof...(Members) > 0) insertImpl<0, Members...>(instance);
        // Or cast to void to shut up static analysis.
        else
            static_cast<void>(instance);
    }

    template<typename IdMember, typename... Members>
    void PrimitiveArrayHandler<IdMember, Members...>::update(const object_t& instance)
    {
        // Delete old values.
        del(IdMember::template get(instance));
        // Insert new values.
        insert(instance);
    }

    template<typename IdMember, typename... Members>
    void PrimitiveArrayHandler<IdMember, Members...>::get(const InstanceId id, object_t& instance)
    {
        // Set ID parameter that is used by select statement(s).
        selectStmtIdParam = id;

        // Call recursive implementation method if there are members.
        if constexpr (sizeof...(Members) > 0) getImpl<0, Members...>(instance);
        // Or cast to void to shut up static analysis.
        else
            static_cast<void>(instance);
    }

    template<typename IdMember, typename... Members>
    void PrimitiveArrayHandler<IdMember, Members...>::del(const InstanceId id)
    {
        // Set ID parameter that is used by delete statement(s).
        deleteStmtIdParam = id;

        // Call recursive implementation method if there are members.
        if constexpr (sizeof...(Members) > 0) delImpl<0, Members...>();
    }

    ////////////////////////////////////////////////////////////////
    // Private utility methods.
    ////////////////////////////////////////////////////////////////

    template<typename IdMember, typename... Members>
    template<size_t I, typename M, typename... Ms>
    void PrimitiveArrayHandler<IdMember, Members...>::initialize(const std::vector<std::string>& names)
    {
        // Create table.
        std::get<I>(tables) = std::tuple_element_t<I, table_t>(database->getTable(names[I]));
        auto& table         = std::get<I>(tables);

        // Create insert statement.
        std::get<I>(insertStmt) = table.insert();

        // Create get statement.
        std::get<I>(selectStmt) = table.template select<type_to_column_t<typename M::value_t>, 2>(
          table.template col<1>() == &selectStmtIdParam, false);

        // Create delete statement.
        std::get<I>(deleteStmt) = table.del(table.template col<1>() == &deleteStmtIdParam, false);

        // Recurse.
        if constexpr (sizeof...(Ms) > 0) initialize<I + 1, Ms...>(names);
    }

    template<typename IdMember, typename... Members>
    template<size_t I, typename M, typename... Ms>
    void PrimitiveArrayHandler<IdMember, Members...>::insertImpl(const object_t& instance)
    {
        // Get primitive array and statement object.
        auto& primitiveArray = M::template get(instance);
        auto& stmt           = std::get<I>(insertStmt);

        // Iterate over primitive array and insert values.
        for (const auto& v : primitiveArray.get())
        {
            // Integral and floating point can be passed to statement directly.
            // String must be put in a wrapper.
            using bind_t = std::remove_reference_t<decltype(v)>;
            if constexpr (std::is_integral_v<bind_t> || std::is_floating_point_v<bind_t>)
                stmt(nullptr, IdMember::template get(instance).get(), v);
            else
                stmt(nullptr, IdMember::template get(instance).get(), sql::toTransientText(v));
        }

        // Recurse.
        if constexpr (sizeof...(Ms) > 0) insertImpl<I + 1, Ms...>(instance);
    }

    template<typename IdMember, typename... Members>
    template<size_t I, typename M, typename... Ms>
    void PrimitiveArrayHandler<IdMember, Members...>::getImpl(object_t& instance)
    {
        // Get primitive array and statement object.
        auto& primitiveArray = M::template get(instance);
        auto& stmt           = std::get<I>(selectStmt);

        // Run statement with rebind and assign to primitive array member.
        primitiveArray.get().assign(stmt(true).begin(), stmt.end());

        // Recurse.
        if constexpr (sizeof...(Ms) > 0) getImpl<I + 1, Ms...>(instance);
    }

    template<typename IdMember, typename... Members>
    template<size_t I, typename M, typename... Ms>
    void PrimitiveArrayHandler<IdMember, Members...>::delImpl()
    {
        // Get statement object.
        auto& stmt = std::get<I>(deleteStmt);

        // Run statement with rebind.
        stmt(true);

        // Recurse.
        if constexpr (sizeof...(Ms) > 0) delImpl<I + 1, Ms...>();
    }

    ////////////////////////////////////////////////////////////////
    // PrimitiveArrayHandler type generator.
    ////////////////////////////////////////////////////////////////

    // Base.
    template<size_t, typename...>
    struct GeneratePrimitiveArrayHandlerImpl
    {
    };

    // No primitive array type, drop M.
    template<size_t I, typename IdMember, typename M, typename... Members>
    struct GeneratePrimitiveArrayHandlerImpl<I, IdMember, M, Members...>
        : GeneratePrimitiveArrayHandlerImpl<I - 1, IdMember, Members...>
    {
    };

    // Primitive array type, append M to end.
    template<size_t I, typename IdMember, typename M, typename... Members>
    requires(M::is_primitive_array ||
             M::is_string_array) struct GeneratePrimitiveArrayHandlerImpl<I, IdMember, M, Members...>
        : GeneratePrimitiveArrayHandlerImpl<I - 1, IdMember, Members..., M>
    {
    };

    // Terminate when I == 0.
    template<typename IdMember, typename... Members>
    struct GeneratePrimitiveArrayHandlerImpl<0, IdMember, Members...>
    {
        using type = PrimitiveArrayHandler<IdMember, Members...>;
    };

    // Terminate when I == 0.
    template<typename IdMember, typename M, typename... Members>
    struct GeneratePrimitiveArrayHandlerImpl<0, IdMember, M, Members...>
    {
        using type = PrimitiveArrayHandler<IdMember, M, Members...>;
    };

    template<typename IdMember, typename... Members>
    struct GeneratePrimitiveArrayHandler
    {
        using type = typename GeneratePrimitiveArrayHandlerImpl<sizeof...(Members), IdMember, Members...>::type;
    };

    /**
     * \brief Takes a list of members and returns a PrimitiveArrayHandler with all non-PrimitiveArray members filtered out.
     * \tparam IdMember ID member.
     * \tparam Members Members.
     */
    template<typename IdMember, typename... Members>
    using generate_primitive_array_handler_t = typename GeneratePrimitiveArrayHandler<IdMember, Members...>::type;
}  // namespace alex