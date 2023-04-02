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
        struct PrimitiveArrayGetterImpl
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

            PrimitiveArrayGetterImpl(const type_descriptor_t&, std::string&) noexcept {}

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
        struct PrimitiveArrayGetterImpl<I, T, std::tuple<M, Ms...>>
            : PrimitiveArrayGetterImpl<I, T, std::tuple<M>>, PrimitiveArrayGetterImpl<I + 1, T, std::tuple<Ms...>>
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

            explicit PrimitiveArrayGetterImpl(const type_descriptor_t& desc, std::string& uuidParam) :
                PrimitiveArrayGetterImpl<I, T, std::tuple<M>>(desc, uuidParam),
                PrimitiveArrayGetterImpl<I + 1, T, std::tuple<Ms...>>(desc, uuidParam)
            {
            }

            ////////////////////////////////////////////////////////////////
            // Invoke.
            ////////////////////////////////////////////////////////////////

            void operator()(object_t& instance)
            {
                // Call implementation for M.
                static_cast<PrimitiveArrayGetterImpl<I, T, std::tuple<M>>&>(*this)(instance);
                // Recurse on Ms...
                static_cast<PrimitiveArrayGetterImpl<I + 1, T, std::tuple<Ms...>>&>(*this)(instance);
            }
        };

        /**
         * \brief Implementation for M.
         * \tparam I Index.
         * \tparam T TypeDescriptor.
         * \tparam M Member.
         */
        template<size_t I, typename T, typename M>
        struct PrimitiveArrayGetterImpl<I, T, std::tuple<M>>
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

            PrimitiveArrayGetterImpl(const type_descriptor_t& desc, std::string& uuidParam) :
                statement(compile(desc, uuidParam))
            {
            }

            ////////////////////////////////////////////////////////////////
            // Invoke.
            ////////////////////////////////////////////////////////////////

            void operator()(object_t& instance)
            {
                auto& array = member_t::template get(instance);
                array.clear();
                for (auto v : statement.bind(sql::BindParameters::Dynamic)) { array.add(std::move(v)); }

                statement.clearBindings();
            }

        private:
            [[nodiscard]] static statement_t compile(const type_descriptor_t& desc, std::string& uuidParam)
            {
                const Type& type   = desc.getType();
                const auto& tables = type.getPrimitiveArrayTables();
                const auto  table  = table_t(*tables[I]);
                // TODO: Should this not be ordered by rowid? Same for other array getters.
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
     * \brief The PrimitiveArrayGetter handles the retrieval of rows from the array table of each PrimitiveArray member.
     * \tparam T TypeDescriptor.
     */
    template<typename T>
    class PrimitiveArrayGetter
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
        using impl_t = detail::PrimitiveArrayGetterImpl<0, type_descriptor_t, members_t>;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        PrimitiveArrayGetter() = delete;

        PrimitiveArrayGetter(const type_descriptor_t& desc, std::string& uuidParam) : impl(desc, uuidParam) {}

        PrimitiveArrayGetter(const PrimitiveArrayGetter&) = delete;

        PrimitiveArrayGetter(PrimitiveArrayGetter&&) = default;

        ~PrimitiveArrayGetter() noexcept = default;

        PrimitiveArrayGetter& operator=(const PrimitiveArrayGetter&) = delete;

        PrimitiveArrayGetter& operator=(PrimitiveArrayGetter&&) = default;

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
