#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <type_traits>

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "alexandria/core/type.h"
#include "alexandria/queries/utils.h"
#include "alexandria/queries/types/member_extractor.h"

namespace alex
{
    namespace detail
    {
        /**
         * \brief General definition.
         * \tparam I Index.
         * \tparam T TypeDescriptor.
         */
        template<size_t I, typename T, typename...>
        struct PrimitiveArrayInserterImpl
        {
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

            ////////////////////////////////////////////////////////////////
            // Constructors.
            ////////////////////////////////////////////////////////////////

            explicit PrimitiveArrayInserterImpl(const type_descriptor_t&) noexcept {}

            ////////////////////////////////////////////////////////////////
            // Invoke.
            ////////////////////////////////////////////////////////////////

            void operator()(object_t&) const noexcept {}
        };

        /**
         * \brief Recursive definition.
         * \tparam I Index.
         * \tparam T TypeDescriptor.
         * \tparam M Current Member.
         * \tparam Ms Remaining Members.
         */
        template<size_t I, typename T, typename M, typename... Ms>
        struct PrimitiveArrayInserterImpl<I, T, std::tuple<M, Ms...>>
            : PrimitiveArrayInserterImpl<I, T, std::tuple<M>>, PrimitiveArrayInserterImpl<I + 1, T, std::tuple<Ms...>>
        {
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

            ////////////////////////////////////////////////////////////////
            // Constructors.
            ////////////////////////////////////////////////////////////////

            explicit PrimitiveArrayInserterImpl(const type_descriptor_t& desc) :
                PrimitiveArrayInserterImpl<I, T, std::tuple<M>>(desc),
                PrimitiveArrayInserterImpl<I + 1, T, std::tuple<Ms...>>(desc)
            {
            }

            ////////////////////////////////////////////////////////////////
            // Invoke.
            ////////////////////////////////////////////////////////////////

            void operator()(object_t& instance)
            {
                // Call implementation for M.
                static_cast<PrimitiveArrayInserterImpl<I, T, std::tuple<M>>&>(*this)(instance);
                // Recurse on Ms...
                static_cast<PrimitiveArrayInserterImpl<I + 1, T, std::tuple<Ms...>>&>(*this)(instance);
            }
        };

        /**
         * \brief Implementation for M.
         * \tparam I Index.
         * \tparam T TypeDescriptor.
         * \tparam M Member.
         */
        template<size_t I, typename T, typename M>
        struct PrimitiveArrayInserterImpl<I, T, std::tuple<M>>
        {
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
             * \brief Member type.
             */
            using member_t = M;

            /**
             * \brief TypedTable for the primitive array table.
             */
            using table_t = primitive_array_table_t<member_t>;

            /**
             * \brief Insert query type.
             */
            using query_t = std::remove_cvref_t<decltype(std::declval<table_t>().insert())>;

            /**
             * \brief Insert statement type.
             */
            using statement_t = std::remove_cvref_t<decltype(std::declval<query_t>().compile())>;

            ////////////////////////////////////////////////////////////////
            // Constructors.
            ////////////////////////////////////////////////////////////////

            explicit PrimitiveArrayInserterImpl(const type_descriptor_t& desc) : statement(compile(desc)) {}

            ////////////////////////////////////////////////////////////////
            // Invoke.
            ////////////////////////////////////////////////////////////////

            void operator()(object_t& instance)
            {
                const std::string uuidstr = type_descriptor_t::uuid_member_t::template get(instance).getAsString();
                const auto        uuid    = sql::toStaticText(uuidstr);

                for (const auto& values = member_t::template get(instance).get(); const auto& v : values)
                {
                    if constexpr (member_t::is_string_array)
                        statement(nullptr, uuid, sql::toStaticText(v));
                    else
                        statement(nullptr, uuid, v);
                }
            }

        private:
            [[nodiscard]] static statement_t compile(const type_descriptor_t& desc)
            {
                const Type& type = desc.getType();
                // TODO: This constructs the same vector for each inserter now. Somewhat inefficient.
                const auto tables = type.getPrimitiveArrayTables();
                const auto table  = table_t(*tables[I]);
                return table.insert().compile();
            }

            ////////////////////////////////////////////////////////////////
            // Member variables.
            ////////////////////////////////////////////////////////////////

            statement_t statement;
        };
    }  // namespace detail

    /**
     * \brief The PrimitiveArrayInserter handles the insertion of PrimitiveArray members into their dedicated array
     * table.
     * \tparam T TypeDescriptor.
     */
    template<typename T>
    class PrimitiveArrayInserter
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
         * \brief List of PrimitiveArray members.
         */
        using members_t = detail::extract_primitive_array_members_t<typename type_descriptor_t::members_t>;

        /**
         * \brief 
         */
        using impl_t = detail::PrimitiveArrayInserterImpl<0, type_descriptor_t, members_t>;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        PrimitiveArrayInserter() = delete;

        explicit PrimitiveArrayInserter(const type_descriptor_t& desc) : impl(desc) {}

        PrimitiveArrayInserter(const PrimitiveArrayInserter&) = delete;

        PrimitiveArrayInserter(PrimitiveArrayInserter&&) = default;

        ~PrimitiveArrayInserter() noexcept = default;

        PrimitiveArrayInserter& operator=(const PrimitiveArrayInserter&) = delete;

        PrimitiveArrayInserter& operator=(PrimitiveArrayInserter&&) = default;

        ////////////////////////////////////////////////////////////////
        // Invoke.
        ////////////////////////////////////////////////////////////////

        void operator()(object_t& instance) { impl(instance); }

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        impl_t impl;
    };
}  // namespace alex
