#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <type_traits>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria-core/type.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "alexandria-query/utils.h"
#include "alexandria-query/types/member_extractor.h"

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
        struct ReferenceArrayInserterImpl
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

            explicit ReferenceArrayInserterImpl(const type_descriptor_t&) noexcept {}

            ////////////////////////////////////////////////////////////////
            // Invoke.
            ////////////////////////////////////////////////////////////////

            void operator()(object_t&, const sql::StaticText&) const noexcept {}
        };

        /**
         * \brief Recursive definition.
         * \tparam I Index.
         * \tparam T TypeDescriptor.
         * \tparam M Current Member.
         * \tparam Ms Remaining Members.
         */
        template<size_t I, typename T, typename M, typename... Ms>
        struct ReferenceArrayInserterImpl<I, T, std::tuple<M, Ms...>>
            : ReferenceArrayInserterImpl<I, T, std::tuple<M>>, ReferenceArrayInserterImpl<I + 1, T, std::tuple<Ms...>>
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

            explicit ReferenceArrayInserterImpl(const type_descriptor_t& desc) :
                ReferenceArrayInserterImpl<I, T, std::tuple<M>>(desc),
                ReferenceArrayInserterImpl<I + 1, T, std::tuple<Ms...>>(desc)
            {
            }

            ////////////////////////////////////////////////////////////////
            // Invoke.
            ////////////////////////////////////////////////////////////////

            void operator()(object_t& instance, const sql::StaticText& uuid)
            {
                // Call implementation for M.
                static_cast<ReferenceArrayInserterImpl<I, T, std::tuple<M>>&>(*this)(instance, uuid);
                // Recurse on Ms...
                static_cast<ReferenceArrayInserterImpl<I + 1, T, std::tuple<Ms...>>&>(*this)(instance, uuid);
            }
        };

        /**
         * \brief Implementation for M.
         * \tparam I Index.
         * \tparam T TypeDescriptor.
         * \tparam M Member.
         */
        template<size_t I, typename T, typename M>
        struct ReferenceArrayInserterImpl<I, T, std::tuple<M>>
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
             * \brief TypedTable for the reference array table.
             */
            using table_t = reference_array_table_t<member_t>;

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

            explicit ReferenceArrayInserterImpl(const type_descriptor_t& desc) : statement(compile(desc)) {}

            ////////////////////////////////////////////////////////////////
            // Invoke.
            ////////////////////////////////////////////////////////////////

            void operator()(object_t& instance, const sql::StaticText& uuid)
            {
                try
                {
                    for (const auto& values = member_t::template get(instance).get(); const auto v : values)
                        statement(nullptr, uuid, sql::toText(v.getAsString()));
                }
                catch (const sql::SqliteError& e)
                {
                    if (e.getExtendedErrorCode() == 787)
                        throw std::runtime_error(std::format(
                          "Failed to insert instance into reference array table. Most likely cause: instance has a "
                          "reference array property that references an object that no longer exists. Internal error "
                          "message: \"{}\"",
                          e.what()));

                    throw std::runtime_error(std::format("Failed to insert instance into reference array table. Most "
                                                         "likely cause unknown. Internal error message: \"{}\"",
                                                         e.what()));
                }
                catch (const std::exception& e)
                {
                    throw std::runtime_error(std::format("Failed to insert instance into reference array table. Most "
                                                         "likely cause unknown. Internal error message: \"{}\"",
                                                         e.what()));
                }

                statement.clearBindings();
            }

        private:
            [[nodiscard]] static statement_t compile(const type_descriptor_t& desc)
            {
                const Type& type   = desc.getType();
                const auto& tables = type.getReferenceArrayTables();
                const auto  table  = table_t(*tables[I]);
                return table.insert().compile();
            }

            ////////////////////////////////////////////////////////////////
            // Member variables.
            ////////////////////////////////////////////////////////////////

            statement_t statement;
        };
    }  // namespace detail

    /**
     * \brief The ReferenceArrayInserter handles the insertion of ReferenceArray members into their dedicated array
     * table.
     * \tparam T TypeDescriptor.
     */
    template<typename T>
    class ReferenceArrayInserter
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
         * \brief List of ReferenceArray members.
         */
        using members_t = detail::extract_reference_array_members_t<typename type_descriptor_t::members_t>;

        /**
         * \brief
         */
        using impl_t = detail::ReferenceArrayInserterImpl<0, type_descriptor_t, members_t>;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        ReferenceArrayInserter() = delete;

        explicit ReferenceArrayInserter(const type_descriptor_t& desc) : impl(desc) {}

        ReferenceArrayInserter(const ReferenceArrayInserter&) = delete;

        ReferenceArrayInserter(ReferenceArrayInserter&&) = default;

        ~ReferenceArrayInserter() noexcept = default;

        ReferenceArrayInserter& operator=(const ReferenceArrayInserter&) = delete;

        ReferenceArrayInserter& operator=(ReferenceArrayInserter&&) = default;

        ////////////////////////////////////////////////////////////////
        // Invoke.
        ////////////////////////////////////////////////////////////////

        void operator()(object_t& instance, const sql::StaticText& uuid) { impl(instance, uuid); }

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        impl_t impl;
    };
}  // namespace alex
