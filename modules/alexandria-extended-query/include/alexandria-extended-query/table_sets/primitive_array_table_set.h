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
        struct PrimitiveArrayTableSetImpl
        {
            explicit PrimitiveArrayTableSetImpl(const T&) {}
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
        struct PrimitiveArrayTableSetImpl<I, T, std::tuple<M, Ms...>>
            : PrimitiveArrayTableSetImpl<I + 1, T, std::tuple<Ms...>>
        {
            using base_t = PrimitiveArrayTableSetImpl<I + 1, T, std::tuple<Ms...>>;
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
             * \brief TypedTable for the primitive array table.
             */
            using table_t = primitive_array_table_t<member_t>;

            ////////////////////////////////////////////////////////////////
            // Constructors.
            ////////////////////////////////////////////////////////////////

            explicit PrimitiveArrayTableSetImpl(const type_descriptor_t& desc) : base_t(desc), table(compile(desc)) {}

            ////////////////////////////////////////////////////////////////
            // Invoke.
            ////////////////////////////////////////////////////////////////

            [[nodiscard]] table_t& get(std::integral_constant<size_t, I>) noexcept { return table; }

        private:
            [[nodiscard]] static table_t compile(const type_descriptor_t& desc)
            {
                const Type& type = desc.getType();
                const auto& tables = type.getPrimitiveArrayTables();
                return table_t(*tables[I]);
            }

            ////////////////////////////////////////////////////////////////
            // Member variables.
            ////////////////////////////////////////////////////////////////

            table_t table;
        };
    }  // namespace detail

    /**
     * \brief The PrimitiveArrayTableSet holds the array table of each PrimitiveArray member.
     * \tparam T TypeDescriptor.
     */
    template<typename T>
    class PrimitiveArrayTableSet
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
        using impl_t = detail::PrimitiveArrayTableSetImpl<0, type_descriptor_t, members_t>;

        static constexpr size_t size = std::tuple_size_v<members_t>;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        PrimitiveArrayTableSet() = delete;

        explicit PrimitiveArrayTableSet(const type_descriptor_t& desc) : impl(desc) {}

        PrimitiveArrayTableSet(const PrimitiveArrayTableSet&) = delete;

        PrimitiveArrayTableSet(PrimitiveArrayTableSet&&) = default;

        ~PrimitiveArrayTableSet() noexcept = default;

        PrimitiveArrayTableSet& operator=(const PrimitiveArrayTableSet&) = delete;

        PrimitiveArrayTableSet& operator=(PrimitiveArrayTableSet&&) = default;

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
