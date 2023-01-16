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
        struct BlobArrayGetterImpl
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

            BlobArrayGetterImpl(const type_descriptor_t&, std::string&) noexcept {}

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
        struct BlobArrayGetterImpl<I, T, std::tuple<M, Ms...>> : BlobArrayGetterImpl<I, T, std::tuple<M>>,
                                                                 BlobArrayGetterImpl<I + 1, T, std::tuple<Ms...>>
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

            explicit BlobArrayGetterImpl(const type_descriptor_t& desc, std::string& uuidParam) :
                BlobArrayGetterImpl<I, T, std::tuple<M>>(desc, uuidParam),
                BlobArrayGetterImpl<I + 1, T, std::tuple<Ms...>>(desc, uuidParam)
            {
            }

            ////////////////////////////////////////////////////////////////
            // Invoke.
            ////////////////////////////////////////////////////////////////

            void operator()(object_t& instance)
            {
                // Call implementation for M.
                static_cast<BlobArrayGetterImpl<I, T, std::tuple<M>>&>(*this)(instance);
                // Recurse on Ms...
                static_cast<BlobArrayGetterImpl<I + 1, T, std::tuple<Ms...>>&>(*this)(instance);
            }
        };

        /**
         * \brief Implementation for M.
         * \tparam I Index.
         * \tparam T TypeDescriptor.
         * \tparam M Member.
         */
        template<size_t I, typename T, typename M>
        struct BlobArrayGetterImpl<I, T, std::tuple<M>>
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
             * \brief TypedTable for the blob array table.
             */
            using table_t = blob_array_table_t<member_t>;

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

            BlobArrayGetterImpl(const type_descriptor_t& desc, std::string& uuidParam) :
                statement(compile(desc, uuidParam))
            {
            }

            ////////////////////////////////////////////////////////////////
            // Invoke.
            ////////////////////////////////////////////////////////////////

            void operator()(object_t& instance)
            {
                auto& blobArray = member_t::template get(instance);
                blobArray.clear();
                for (auto v : statement.bind(sql::BindParameters::Dynamic)) { blobArray.add(std::move(v)); }
                statement.clearBindings();
            }

        private:
            [[nodiscard]] static statement_t compile(const type_descriptor_t& desc, std::string& uuidParam)
            {
                const Type& type = desc.getType();
                // TODO: This constructs the same vector for each inserter now. Somewhat inefficient.
                const auto tables = type.getBlobArrayTables();
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
     * \brief The BlobArrayGetter handles the retrieval of rows from the array table of each BlobArray member.
     * \tparam T TypeDescriptor.
     */
    template<typename T>
    class BlobArrayGetter
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
         * \brief List of BlobArray members.
         */
        using members_t = detail::extract_blob_array_members_t<typename type_descriptor_t::members_t>;

        /**
         * \brief
         */
        using impl_t = detail::BlobArrayGetterImpl<0, type_descriptor_t, members_t>;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        BlobArrayGetter() = delete;

        BlobArrayGetter(const type_descriptor_t& desc, std::string& uuidParam) : impl(desc, uuidParam) {}

        BlobArrayGetter(const BlobArrayGetter&) = delete;

        BlobArrayGetter(BlobArrayGetter&&) = default;

        ~BlobArrayGetter() noexcept = default;

        BlobArrayGetter& operator=(const BlobArrayGetter&) = delete;

        BlobArrayGetter& operator=(BlobArrayGetter&&) = default;

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
