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
        struct ReferenceArrayGetterImpl
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

            ReferenceArrayGetterImpl(const type_descriptor_t&, std::string&) noexcept {}

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
        struct ReferenceArrayGetterImpl<I, T, std::tuple<M, Ms...>>
            : ReferenceArrayGetterImpl<I, T, std::tuple<M>>, ReferenceArrayGetterImpl<I + 1, T, std::tuple<Ms...>>
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

            explicit ReferenceArrayGetterImpl(const type_descriptor_t& desc, std::string& uuidParam) :
                ReferenceArrayGetterImpl<I, T, std::tuple<M>>(desc, uuidParam),
                ReferenceArrayGetterImpl<I + 1, T, std::tuple<Ms...>>(desc, uuidParam)
            {
            }

            ////////////////////////////////////////////////////////////////
            // Invoke.
            ////////////////////////////////////////////////////////////////

            void operator()(object_t& instance)
            {
                // Call implementation for M.
                static_cast<ReferenceArrayGetterImpl<I, T, std::tuple<M>>&>(*this)(instance);
                // Recurse on Ms...
                static_cast<ReferenceArrayGetterImpl<I + 1, T, std::tuple<Ms...>>&>(*this)(instance);
            }
        };

        /**
         * \brief Implementation for M.
         * \tparam I Index.
         * \tparam T TypeDescriptor.
         * \tparam M Member.
         */
        template<size_t I, typename T, typename M>
        struct ReferenceArrayGetterImpl<I, T, std::tuple<M>>
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
             * \brief Select query type.
             */
            using query_t =
              std::remove_cvref_t<decltype(std::declval<table_t>().template selectAs<sql::col_t<2, table_t>, 2>())>;

            /**
             * \brief Select statement type.
             */
            using statement_t = std::remove_cvref_t<decltype(std::declval<query_t>().compile())>;

            ////////////////////////////////////////////////////////////////
            // Constructors.
            ////////////////////////////////////////////////////////////////

            explicit ReferenceArrayGetterImpl(const type_descriptor_t& desc, std::string& uuidParam) :
                statement(compile(desc, uuidParam))
            {
            }

            ////////////////////////////////////////////////////////////////
            // Invoke.
            ////////////////////////////////////////////////////////////////

            void operator()(object_t& instance)
            {
                auto& container = member_t::template get(instance).get();
                container.clear();
                for (auto v : statement.bind(sql::BindParameters::Dynamic)) { container.emplace_back(std::move(v)); }
            }

        private:
            [[nodiscard]] static statement_t compile(const type_descriptor_t& desc, std::string& uuidParam)
            {
                const Type& type = desc.getType();
                // TODO: This constructs the same vector for each inserter now. Somewhat inefficient.
                const auto tables = type.getReferenceArrayTables();
                const auto table  = table_t(*tables[I]);
                return table.template selectAs<sql::col_t<2, table_t>, 2>()
                  .where(sql::like(table.template col<1>(), &uuidParam))
                  .compile();
            }

            ////////////////////////////////////////////////////////////////
            // Member variables.
            ////////////////////////////////////////////////////////////////

            statement_t statement;
        };
    }  // namespace detail

    /**
     * \brief The ReferenceArrayGetter handles the retrieval of rows from the array table of each ReferenceArray member.
     * \tparam T TypeDescriptor.
     */
    template<typename T>
    class ReferenceArrayGetter
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
        using impl_t = detail::ReferenceArrayGetterImpl<0, type_descriptor_t, members_t>;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        ReferenceArrayGetter() = delete;

        ReferenceArrayGetter(const type_descriptor_t& desc, std::string& uuidParam) : impl(desc, uuidParam) {}

        ReferenceArrayGetter(const ReferenceArrayGetter&) = delete;

        ReferenceArrayGetter(ReferenceArrayGetter&&) = default;

        ~ReferenceArrayGetter() noexcept = default;

        ReferenceArrayGetter& operator=(const ReferenceArrayGetter&) = delete;

        ReferenceArrayGetter& operator=(ReferenceArrayGetter&&) = default;

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
