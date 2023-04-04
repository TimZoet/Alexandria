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

#include "alexandria-basic-query/utils.h"
#include "alexandria-basic-query/types/member_extractor.h"

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
        struct BlobArrayDeleterImpl
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

            BlobArrayDeleterImpl(const type_descriptor_t&, std::string&) noexcept {}

            ////////////////////////////////////////////////////////////////
            // Invoke.
            ////////////////////////////////////////////////////////////////

            void operator()() const noexcept {}
        };

        /**
         * \brief Recursive definition.
         * \tparam I Index.
         * \tparam T TypeDescriptor.
         * \tparam M Current Member.
         * \tparam Ms Remaining Members.
         */
        template<size_t I, typename T, typename M, typename... Ms>
        struct BlobArrayDeleterImpl<I, T, std::tuple<M, Ms...>> : BlobArrayDeleterImpl<I, T, std::tuple<M>>,
                                                                  BlobArrayDeleterImpl<I + 1, T, std::tuple<Ms...>>
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

            explicit BlobArrayDeleterImpl(const type_descriptor_t& desc, std::string& uuidParam) :
                BlobArrayDeleterImpl<I, T, std::tuple<M>>(desc, uuidParam),
                BlobArrayDeleterImpl<I + 1, T, std::tuple<Ms...>>(desc, uuidParam)
            {
            }

            ////////////////////////////////////////////////////////////////
            // Invoke.
            ////////////////////////////////////////////////////////////////

            void operator()()
            {
                // Call implementation for M.
                static_cast<BlobArrayDeleterImpl<I, T, std::tuple<M>>&>(*this)();
                // Recurse on Ms...
                static_cast<BlobArrayDeleterImpl<I + 1, T, std::tuple<Ms...>>&>(*this)();
            }
        };

        /**
         * \brief Implementation for M.
         * \tparam I Index.
         * \tparam T TypeDescriptor.
         * \tparam M Member.
         */
        template<size_t I, typename T, typename M>
        struct BlobArrayDeleterImpl<I, T, std::tuple<M>>
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
             * \brief Delete query type.
             */
            using query_t = std::remove_cvref_t<decltype(std::declval<table_t>().del())>;

            /**
             * \brief Delete statement type.
             */
            using statement_t = std::remove_cvref_t<decltype(std::declval<query_t>().compile())>;

            ////////////////////////////////////////////////////////////////
            // Constructors.
            ////////////////////////////////////////////////////////////////

            BlobArrayDeleterImpl(const type_descriptor_t& desc, std::string& uuidParam) :
                statement(compile(desc, uuidParam))
            {
            }

            ////////////////////////////////////////////////////////////////
            // Invoke.
            ////////////////////////////////////////////////////////////////

            void operator()()
            {
                statement.bind(sql::BindParameters::Dynamic)();
                statement.clearBindings();
            }

        private:
            [[nodiscard]] static statement_t compile(const type_descriptor_t& desc, std::string& uuidParam)
            {
                const Type& type   = desc.getType();
                const auto& tables = type.getBlobArrayTables();
                const auto  table  = table_t(*tables[I]);
                return table.del().where(sql::like(table.template col<1>(), &uuidParam)).compile();
            }

            ////////////////////////////////////////////////////////////////
            // Member variables.
            ////////////////////////////////////////////////////////////////

            statement_t statement;
        };
    }  // namespace detail

    /**
     * \brief The BlobArrayDeleter handles the removal of rows from the array table of each BlobArray member.
     * \tparam T TypeDescriptor.
     */
    template<typename T>
    class BlobArrayDeleter
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
        using impl_t = detail::BlobArrayDeleterImpl<0, type_descriptor_t, members_t>;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        BlobArrayDeleter() = delete;

        BlobArrayDeleter(const type_descriptor_t& desc, std::string& uuidParam) : impl(desc, uuidParam) {}

        BlobArrayDeleter(const BlobArrayDeleter&) = delete;

        BlobArrayDeleter(BlobArrayDeleter&&) = default;

        ~BlobArrayDeleter() noexcept = default;

        BlobArrayDeleter& operator=(const BlobArrayDeleter&) = delete;

        BlobArrayDeleter& operator=(BlobArrayDeleter&&) = default;

        ////////////////////////////////////////////////////////////////
        // Invoke.
        ////////////////////////////////////////////////////////////////

        void operator()() { impl(); }

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        impl_t impl;
    };
}  // namespace alex
