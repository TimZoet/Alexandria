#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <type_traits>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "common/type_traits.h"
#include "cppql-core/database.h"
#include "cppql-typed/queries/count.h"
#include "cppql-typed/queries/delete.h"
#include "cppql-typed/queries/insert.h"
#include "cppql-typed/queries/select_one.h"
#include "cppql-typed/queries/update.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "alexandria/type.h"
#include "alexandria/member_types/member.h"
#include "alexandria/object_handler/type_traits.h"

namespace alex
{
    ////////////////////////////////////////////////////////////////
    // PrimitiveHandler type traits.
    ////////////////////////////////////////////////////////////////

    /**
     * \brief Generates a TypedTable. For each member a column is added to the table.
     * \tparam Ms Members.
     */
    template<typename... Ms>
    using primitive_table_t = sql::TypedTable<InstanceId::type, type_to_column_t<typename Ms::value_t>...>;

    template<typename T>
    using primitive_create_t = decltype(std::declval<T>().template insert<>());

    /**
     * \brief Generates an Insert type.
     * \tparam T TypedTable.
     */
    template<typename T>
    using primitive_insert_t = sql::insert_t<T>;

    /**
     * \brief Generates an Update type.
     * \tparam T TypedTable.
     */
    template<typename T>
    using primitive_update_t = sql::update_t<T>;

    /**
     * \brief Generates a Select type.
     * \tparam T TypedTable.
     */
    template<typename T>
    using primitive_select_t = sql::select_one3_t<T>;

    /**
     * \brief Generates a Delete type.
     * \tparam T TypedTable.
     */
    template<typename T>
    using primitive_delete_t = sql::delete_t<T>;

    /**
     * \brief Generates a Count type.
     * \tparam T TypedTable.
     */
    template<typename T>
    using primitive_count_t = sql::count_t<T>;

    /**
     * \brief Generates a SelectAll InstanceID type.
     * \tparam T TypedTable.
     */
    template<typename T>
    using primitive_list_t = sql::select_all_t<T, InstanceId, 0>;

    ////////////////////////////////////////////////////////////////
    // PrimitiveHandler class.
    ////////////////////////////////////////////////////////////////

    /**
     * \brief The PrimitiveHandler deals with all primitive member types, as well as e.g. BLOBs that have a single column in the main instance table.
     * \tparam IdMember ID member.
     * \tparam Members Primitive members.
     */
    template<typename IdMember, typename... Members>
    class PrimitiveHandler
    {
    public:
        using object_t             = typename IdMember::class_t;
        using table_t              = primitive_table_t<Members...>;
        using create_stmt_t        = primitive_create_t<table_t>;
        using insert_stmt_t        = primitive_insert_t<table_t>;
        using update_stmt_t        = primitive_update_t<table_t>;
        using select_stmt_t        = primitive_select_t<table_t>;
        using select_stmt_return_t = typename select_stmt_t::select_t::return_t;
        using delete_stmt_t        = primitive_delete_t<table_t>;
        using count_stmt_t         = primitive_count_t<table_t>;
        using list_stmt_t          = primitive_list_t<table_t>;

        PrimitiveHandler() = delete;

        PrimitiveHandler(const Type& t, sql::Database& db);

        PrimitiveHandler(const PrimitiveHandler&) = delete;

        PrimitiveHandler(PrimitiveHandler&&) = delete;

        PrimitiveHandler& operator=(const PrimitiveHandler&) = delete;

        PrimitiveHandler& operator=(PrimitiveHandler&&) = delete;

        ~PrimitiveHandler() noexcept = default;

        ////////////////////////////////////////////////////////////////
        // Main public methods.
        ////////////////////////////////////////////////////////////////

        InstanceId create();

        void insert(object_t& instance);

        void update(const object_t& instance);

        void get(InstanceId id, object_t& instance);

        bool exists(InstanceId id);

        std::vector<InstanceId> list();

        void del(InstanceId id);

        table_t& getTable() noexcept { return table; }

        const table_t& getTable() const noexcept { return table; }

        template<typename C>
        static constexpr size_t getIndex()
        {
            return getIndexImpl<0, C, IdMember, Members...>();
        }

        ////////////////////////////////////////////////////////////////
        // Private utility methods.
        ////////////////////////////////////////////////////////////////

    private:
        template<typename M>
        auto getMemberValue(object_t& instance);

        template<typename M>
        const auto getMemberValue(const object_t& instance);

        template<size_t I, typename M>
        void setMemberValue(object_t& instance, select_stmt_return_t& row);

        template<size_t I, typename M, typename... Ms>
        void getImpl(object_t& instance, select_stmt_return_t& row);

        template<size_t I, typename C, typename M, typename... Ms>
        static constexpr size_t getIndexImpl()
        {
            if constexpr (std::same_as<C, M>)
                return I;
            else if constexpr (sizeof...(Ms) > 0)
                return getIndexImpl<I + 1, C, Ms...>();
            else
                return static_cast<size_t>(-1);
        }

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Type.
         */
        const Type* type;

        /**
         * \brief Database.
         */
        sql::Database* database;

        /**
         * \brief Main instance table.
         */
        table_t table;

        create_stmt_t createStmt;

        /**
         * \brief Statement object to insert new instances.
         */
        insert_stmt_t insertStmt;

        /**
         * \brief Statement object to update instances.
         */
        update_stmt_t updateStmt;

        /**
         * \brief ID parameter that is used to update instances.
         */
        InstanceId::type updateStmtIdParam = 0;

        /**
         * \brief Statement object to retrieve instances.
         */
        select_stmt_t selectStmt;

        /**
         * \brief ID parameter that is used to select instances.
         */
        InstanceId::type selectStmtIdParam = 0;

        /**
         * \brief Statement object to delete instances.
         */
        delete_stmt_t deleteStmt;

        /**
         * \brief ID parameter that is used to delete instances.
         */
        InstanceId::type deleteStmtIdParam = 0;

        /**
         * \brief Statement object to count instances.
         */
        count_stmt_t countStmt;

        /**
         * \brief ID parameter that is used to count instances.
         */
        InstanceId::type countStmtIdParam = 0;

        /**
         * \brief Statement object to retrieve list of all InstanceIDs.
         */
        list_stmt_t listStmt;
    };

    ////////////////////////////////////////////////////////////////
    // Implementation.
    ////////////////////////////////////////////////////////////////

    template<typename IdMember, typename... Members>
    PrimitiveHandler<IdMember, Members...>::PrimitiveHandler(const Type& t, sql::Database& db) :
        type(&t),
        database(&db),
        table(db.getTable(type->getName())),
        createStmt(table.template insert<>()),
        insertStmt(table.insert()),
        updateStmt(table.update(table.template col<0>() == &updateStmtIdParam, false)),
        selectStmt(table.selectOne(table.template col<0>() == &selectStmtIdParam, false)),
        deleteStmt(table.del(table.template col<0>() == &deleteStmtIdParam, false)),
        countStmt(table.count(table.template col<0>() == &countStmtIdParam, false)),
        listStmt(table.template selectAll<InstanceId, 0>())
    {
    }

    ////////////////////////////////////////////////////////////////
    // Main public methods.
    ////////////////////////////////////////////////////////////////

    template<typename IdMember, typename... Members>
    InstanceId PrimitiveHandler<IdMember, Members...>::create()
    {
        // Execute create statement.
        createStmt();

        // Return ID of new instance.
        return InstanceId(database->getLastInsertRowId());
    }

    template<typename IdMember, typename... Members>
    void PrimitiveHandler<IdMember, Members...>::insert(object_t& instance)
    {
        // Execute insert statement.
        insertStmt(nullptr, getMemberValue<Members>(instance)...);
    }

    template<typename IdMember, typename... Members>
    void PrimitiveHandler<IdMember, Members...>::update(const object_t& instance)
    {
        // Set ID parameter that is used by update statement.
        updateStmtIdParam = IdMember::template get(instance);

        // TODO: This unnecessarily updates the ID.
        // Execute update statement.
        updateStmt(true, IdMember::template get(instance).get(), getMemberValue<Members>(instance)...);
        // TODO: Verify number of updated rows == 1. Throw of not. Also look at other handlers to see how they deal with this.
    }

    template<typename IdMember, typename... Members>
    void PrimitiveHandler<IdMember, Members...>::get(const InstanceId id, object_t& instance)
    {
        // Set ID parameter that is used by select statement.
        selectStmtIdParam = id;

        // Run select.
        auto row = selectStmt(true);

        // Assign values to instance.
        getImpl<0, IdMember, Members...>(instance, row);
    }

    template<typename IdMember, typename... Members>
    bool PrimitiveHandler<IdMember, Members...>::exists(const InstanceId id)
    {
        // Set ID parameter that is used by count statement.
        countStmtIdParam = id;

        // Run count.
        return countStmt(true) > 0;
    }

    template<typename IdMember, typename... Members>
    std::vector<InstanceId> PrimitiveHandler<IdMember, Members...>::list()
    {
        return std::vector<InstanceId>(listStmt.begin(), listStmt.end());
    }

    template<typename IdMember, typename... Members>
    void PrimitiveHandler<IdMember, Members...>::del(const InstanceId id)
    {
        // Set ID parameter that is used by delete statement.
        deleteStmtIdParam = id;

        // Run delete.
        deleteStmt(true);
    }

    ////////////////////////////////////////////////////////////////
    // Private utility methods.
    ////////////////////////////////////////////////////////////////

    template<typename IdMember, typename... Members>
    template<typename M>
    auto PrimitiveHandler<IdMember, Members...>::getMemberValue(object_t& instance)
    {
        if constexpr (M::is_blob || M::is_primitive_blob)
            return M::template get(instance).getStaticBlob();
        else if constexpr (M::is_reference)
            // TODO: What if no object was assigned? Turn return type into std::optional and rely on cppql to insert nullptr?
            return M::template get(instance).getId().get();
        else if constexpr (M::is_primitive)
            return M::template get(instance);
        else if constexpr (M::is_string)
            return sql::toTransientText(M::template get(instance));
        else
            constexpr_static_assert();
    }

    template<typename IdMember, typename... Members>
    template<typename M>
    const auto PrimitiveHandler<IdMember, Members...>::getMemberValue(const object_t& instance)
    {
        if constexpr (M::is_blob || M::is_primitive_blob)
            return M::template get(instance).getStaticBlob();
        else if constexpr (M::is_reference)
            // TODO: What if no object was assigned? Turn return type into std::optional and rely on cppql to insert nullptr?
            return M::template get(instance).getId().get();
        else if constexpr (M::is_primitive)
            return M::template get(instance);
        else if constexpr (M::is_string)
            return sql::toTransientText(M::template get(instance));
        else
            constexpr_static_assert();
    }

    template<typename IdMember, typename... Members>
    template<size_t Index, typename M>
    void PrimitiveHandler<IdMember, Members...>::setMemberValue(object_t& instance, select_stmt_return_t& row)
    {
        if constexpr (M::is_blob)
            M::template get(instance).set(std::get<Index>(row));
        else if constexpr (M::is_primitive_blob)
            M::template get(instance).set(std::get<Index>(row));
        else if constexpr (M::is_primitive)
            M::template get(instance) = std::get<Index>(row);
        else if constexpr (M::is_reference)
            M::template get(instance) = InstanceId(std::get<Index>(row));
        else if constexpr (M::is_string)
            M::template get(instance) = std::move(std::get<Index>(row));
        else
            constexpr_static_assert();
    }

    template<typename IdMember, typename... Members>
    template<size_t I, typename M, typename... Ms>
    void PrimitiveHandler<IdMember, Members...>::getImpl(object_t& instance, select_stmt_return_t& row)
    {
        // Retrieve value and assign to instance.
        setMemberValue<I, M>(instance, row);
        // Recurse.
        if constexpr (sizeof...(Ms) > 0) getImpl<I + 1, Ms...>(instance, row);
    }

    ////////////////////////////////////////////////////////////////
    // PrimitiveHandler type generator.
    ////////////////////////////////////////////////////////////////

    template<size_t, typename...>
    struct GeneratePrimitiveHandlerImpl
    {
    };

    // No primitive type, drop M.
    template<size_t I, typename IdMember, typename M, typename... Members>
    struct GeneratePrimitiveHandlerImpl<I, IdMember, M, Members...>
        : GeneratePrimitiveHandlerImpl<I - 1, IdMember, Members...>
    {
    };

    // Primitive type, append M to end.
    template<size_t I, typename IdMember, typename M, typename... Members>
    requires(M::is_primitive || M::is_string || M::is_blob || M::is_reference ||
             M::is_primitive_blob) struct GeneratePrimitiveHandlerImpl<I, IdMember, M, Members...>
        : GeneratePrimitiveHandlerImpl<I - 1, IdMember, Members..., M>
    {
    };

    // Terminate when I == 0.
    template<typename IdMember, typename... Members>
    struct GeneratePrimitiveHandlerImpl<0, IdMember, Members...>
    {
        using type = PrimitiveHandler<IdMember, Members...>;
    };
    
    // Terminate when I == 0.
    // Note: The one termination definition above is not enough. When the original first member was a primitive type,
    // it ends up at the head again. This causes the compiler to pick the append definition again, with I now set to
    // -1 unsigned, causing an endless loop. Just this definition is also not enough, because then empty lists are not
    // handled.
    template<typename IdMember, typename M, typename... Members>
    struct GeneratePrimitiveHandlerImpl<0, IdMember, M, Members...>
    {
        using type = PrimitiveHandler<IdMember, M, Members...>;
    };

    template<typename IdMember, typename... Members>
    struct GeneratePrimitiveHandler
    {
        using type = typename GeneratePrimitiveHandlerImpl<sizeof...(Members), IdMember, Members...>::type;
    };

    /**
     * \brief Takes a list of members and returns a PrimitiveHandler with all non-primitive members filtered out.
     * \tparam IdMember ID member.
     * \tparam Members Members.
     */
    template<typename IdMember, typename... Members>
    using generate_primitive_handler_t = typename GeneratePrimitiveHandler<IdMember, Members...>::type;
}  // namespace alex