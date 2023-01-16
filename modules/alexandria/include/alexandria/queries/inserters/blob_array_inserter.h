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
        struct BlobArrayInserterImpl
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

            explicit BlobArrayInserterImpl(const type_descriptor_t&) noexcept {}

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
        struct BlobArrayInserterImpl<I, T, std::tuple<M, Ms...>> : BlobArrayInserterImpl<I, T, std::tuple<M>>,
                                                                   BlobArrayInserterImpl<I + 1, T, std::tuple<Ms...>>
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

            explicit BlobArrayInserterImpl(const type_descriptor_t& desc) :
                BlobArrayInserterImpl<I, T, std::tuple<M>>(desc),
                BlobArrayInserterImpl<I + 1, T, std::tuple<Ms...>>(desc)
            {
            }

            ////////////////////////////////////////////////////////////////
            // Invoke.
            ////////////////////////////////////////////////////////////////

            void operator()(object_t& instance)
            {
                // Call implementation for M.
                static_cast<BlobArrayInserterImpl<I, T, std::tuple<M>>&>(*this)(instance);
                // Recurse on Ms...
                static_cast<BlobArrayInserterImpl<I + 1, T, std::tuple<Ms...>>&>(*this)(instance);
            }
        };

        /**
         * \brief Implementation for M.
         * \tparam I Index.
         * \tparam T TypeDescriptor.
         * \tparam M Member.
         */
        template<size_t I, typename T, typename M>
        struct BlobArrayInserterImpl<I, T, std::tuple<M>>
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

            explicit BlobArrayInserterImpl(const type_descriptor_t& desc) : statement(compile(desc)) {}

            ////////////////////////////////////////////////////////////////
            // Invoke.
            ////////////////////////////////////////////////////////////////

            void operator()(object_t& instance)
            {
                const std::string uuidstr   = type_descriptor_t::uuid_member_t::template get(instance).getAsString();
                const auto        uuid      = sql::toStaticText(uuidstr);
                const auto&       blobArray = member_t::template get(instance);

                // clang-format off
                if constexpr (requires { { blobArray.getStaticBlob(std::declval<size_t>()) } -> std::same_as<sql::StaticBlob>;})
                    for (size_t i = 0; i < blobArray.size(); i++) statement(nullptr, uuid, blobArray.getStaticBlob(i));
                else if constexpr (requires { { blobArray.getTransientBlob(std::declval<size_t>()) } -> std::same_as<sql::TransientBlob>; })
                    for (size_t i = 0; i < blobArray.size(); i++) statement(nullptr, uuid, blobArray.getTransientBlob(i));
                else
                    for (size_t i = 0; i < blobArray.size(); i++) statement(nullptr, uuid, blobArray.getBlob(i));
                // clang-format on

                statement.clearBindings();
            }

        private:
            [[nodiscard]] static statement_t compile(const type_descriptor_t& desc)
            {
                const Type& type = desc.getType();
                // TODO: This constructs the same vector for each inserter now. Somewhat inefficient.
                const auto tables = type.getBlobArrayTables();
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
     * \brief The BlobArrayInserter handles the insertion of BlobArray members into their dedicated array
     * table.
     * \tparam T TypeDescriptor.
     */
    template<typename T>
    class BlobArrayInserter
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
        using impl_t = detail::BlobArrayInserterImpl<0, type_descriptor_t, members_t>;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        BlobArrayInserter() = delete;

        explicit BlobArrayInserter(const type_descriptor_t& desc) : impl(desc) {}

        BlobArrayInserter(const BlobArrayInserter&) = delete;

        BlobArrayInserter(BlobArrayInserter&&) = default;

        ~BlobArrayInserter() noexcept = default;

        BlobArrayInserter& operator=(const BlobArrayInserter&) = delete;

        BlobArrayInserter& operator=(BlobArrayInserter&&) = default;

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
