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
        struct ReferenceArrayTableSetImpl
        {
            explicit ReferenceArrayTableSetImpl(const T&) {}
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
        struct ReferenceArrayTableSetImpl<I, T, std::tuple<M, Ms...>>
            : ReferenceArrayTableSetImpl<I + 1, T, std::tuple<Ms...>>
        {
            using base_t = ReferenceArrayTableSetImpl<I + 1, T, std::tuple<Ms...>>;
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
             * \brief TypedTable for the reference array table.
             */
            using table_t = reference_array_table_t<member_t>;

            ////////////////////////////////////////////////////////////////
            // Constructors.
            ////////////////////////////////////////////////////////////////

            explicit ReferenceArrayTableSetImpl(const type_descriptor_t& desc) : base_t(desc), table(compile(desc)) {}

            ////////////////////////////////////////////////////////////////
            // Invoke.
            ////////////////////////////////////////////////////////////////

            [[nodiscard]] table_t& get(std::integral_constant<size_t, I>) noexcept { return table; }

        private:
            [[nodiscard]] static table_t compile(const type_descriptor_t& desc)
            {
                const Type& type   = desc.getType();
                const auto& tables = type.getReferenceArrayTables();
                return table_t(*tables[I]);
            }

            ////////////////////////////////////////////////////////////////
            // Member variables.
            ////////////////////////////////////////////////////////////////

            table_t table;
        };
    }  // namespace detail

    /**
     * \brief The ReferenceArrayTableSet holds the array table of each ReferenceArray member.
     * \tparam T TypeDescriptor.
     */
    template<typename T>
    class ReferenceArrayTableSet
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
        using impl_t = detail::ReferenceArrayTableSetImpl<0, type_descriptor_t, members_t>;

        static constexpr size_t size = std::tuple_size_v<members_t>;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        ReferenceArrayTableSet() = delete;

        explicit ReferenceArrayTableSet(const type_descriptor_t& desc) : impl(desc) {}

        ReferenceArrayTableSet(const ReferenceArrayTableSet&) = delete;

        ReferenceArrayTableSet(ReferenceArrayTableSet&&) = default;

        ~ReferenceArrayTableSet() noexcept = default;

        ReferenceArrayTableSet& operator=(const ReferenceArrayTableSet&) = delete;

        ReferenceArrayTableSet& operator=(ReferenceArrayTableSet&&) = default;

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
