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
        struct BlobArrayTableSetImpl
        {
            explicit BlobArrayTableSetImpl(const T&) {}
            static void get() noexcept {}
        };

        /**
         * \brief Recursive definition.
         * \tparam I Index.
         * \tparam T TypeDescriptor.
         * \tparam M Current Member.
         * \tparam Ms Remaining Members.
         */
        template<size_t I, typename T, typename M, typename... Ms>
        struct BlobArrayTableSetImpl<I, T, std::tuple<M, Ms...>> : BlobArrayTableSetImpl<I + 1, T, std::tuple<Ms...>>
        {
            using base_t = BlobArrayTableSetImpl<I + 1, T, std::tuple<Ms...>>;
            using base_t::get;

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

            ////////////////////////////////////////////////////////////////
            // Constructors.
            ////////////////////////////////////////////////////////////////

            explicit BlobArrayTableSetImpl(const type_descriptor_t& desc) : base_t(desc), table(compile(desc)) {}

            ////////////////////////////////////////////////////////////////
            // Getters.
            ////////////////////////////////////////////////////////////////

            [[nodiscard]] table_t& get(std::integral_constant<size_t, I>) noexcept { return table; }

        private:
            [[nodiscard]] static table_t compile(const type_descriptor_t& desc)
            {
                const Type& type   = desc.getType();
                const auto& tables = type.getBlobArrayTables();
                return table_t(*tables[I]);
            }

            ////////////////////////////////////////////////////////////////
            // Member variables.
            ////////////////////////////////////////////////////////////////

            table_t table;
        };
    }  // namespace detail

    /**
     * \brief The BlobArrayTableSet holds the array table of each BlobArray member.
     * \tparam T TypeDescriptor.
     */
    template<typename T>
    class BlobArrayTableSet
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
        using impl_t = detail::BlobArrayTableSetImpl<0, type_descriptor_t, members_t>;

        static constexpr size_t size = std::tuple_size_v<members_t>;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        BlobArrayTableSet() = delete;

        explicit BlobArrayTableSet(const type_descriptor_t& desc) : impl(desc) {}

        BlobArrayTableSet(const BlobArrayTableSet&) = delete;

        BlobArrayTableSet(BlobArrayTableSet&&) = default;

        ~BlobArrayTableSet() noexcept = default;

        BlobArrayTableSet& operator=(const BlobArrayTableSet&) = delete;

        BlobArrayTableSet& operator=(BlobArrayTableSet&&) = default;

        ////////////////////////////////////////////////////////////////
        // Invoke.
        ////////////////////////////////////////////////////////////////

        template<size_t I>
            requires(I < size)
        auto& get() noexcept
        {
            return this->impl.get(std::integral_constant<size_t, I>{});
        }

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        impl_t impl;
    };
}  // namespace alex
