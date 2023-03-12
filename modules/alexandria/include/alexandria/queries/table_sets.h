#pragma once

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "alexandria/queries/table_sets/blob_array_table_set.h"
#include "alexandria/queries/table_sets/primitive_array_table_set.h"
#include "alexandria/queries/table_sets/primitive_table_set.h"
#include "alexandria/queries/table_sets/reference_array_table_set.h"

namespace alex
{
    /**
     * \brief
     * \tparam T TypeDescriptor.
     */
    template<typename T>
    class TableSets
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        using type_descriptor_t           = T;
        using object_t                    = typename type_descriptor_t::object_t;
        using primitive_table_set_t       = PrimitiveTableSet<type_descriptor_t>;
        using primitive_array_table_set_t = PrimitiveArrayTableSet<type_descriptor_t>;
        using blob_array_table_set_t      = BlobArrayTableSet<type_descriptor_t>;
        using reference_array_table_set_t = ReferenceArrayTableSet<type_descriptor_t>;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        explicit TableSets(type_descriptor_t desc) :
            descriptor(desc),
            primitiveTableSet(desc),
            primitiveArrayTableSet(desc),
            blobArrayTableSet(desc),
            referenceArrayTableSet(desc)
        {
        }

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Get instance table.
         * \return Instance table.
         */
        auto& getInstanceTable() noexcept { return primitiveTableSet.get(); }

        /**
         * \brief Get instance table column by index.
         * \tparam Name Member name.
         * \return Instance table column.
         */
        template<detail::MemberName Name>
        [[nodiscard]] auto getInstanceColumn() noexcept
        {
            return primitiveTableSet.template col<Name>();
        }

        /**
         * \brief Get primitive array table by index.
         * \tparam I Index.
         * \return Primitive array table.
         */
        template<size_t I>
            requires(I < primitive_array_table_set_t::size)
        auto& getPrimitiveArrayTable() noexcept
        {
            return primitiveArrayTableSet.template get<I>();
        }

        /**
         * \brief Get primitive array table from a member name.
         * \tparam Name Member name.
         * \return Primitive array table.
         */
        template<detail::MemberName Name>
        auto& getPrimitiveArrayTable() noexcept
        {
            return getPrimitiveArrayTable<
              detail::getColumnIndex<Name, typename primitive_array_table_set_t::members_t>()>();
        }

        /**
         * \brief Get blob array table by index.
         * \tparam I Index.
         * \return Blob array table.
         */
        template<size_t I>
            requires(I < blob_array_table_set_t::size)
        auto& getBlobArrayTable() noexcept
        {
            return blobArrayTableSet.template get<I>();
        }

        /**
         * \brief Get blob array table from a member name.
         * \tparam Name Member name.
         * \return Blob array table.
         */
        template<detail::MemberName Name>
        auto& getBlobArrayTable() noexcept
        {
            return getBlobArrayTable<detail::getColumnIndex<Name, typename blob_array_table_set_t::members_t>()>();
        }

        /**
         * \brief Get reference array table by index.
         * \tparam I Index.
         * \return Reference array table.
         */
        template<size_t I>
            requires(I < reference_array_table_set_t::size)
        auto& getReferenceArrayTable() noexcept
        {
            return referenceArrayTableSet.template get<I>();
        }

        /**
         * \brief Get reference array table from a member name.
         * \tparam Name Member name.
         * \return Reference array table.
         */
        template<detail::MemberName Name>
        auto& getReferenceArrayTable() noexcept
        {
            return getReferenceArrayTable<
              detail::getColumnIndex<Name, typename reference_array_table_set_t::members_t>()>();
        }

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        type_descriptor_t           descriptor;
        primitive_table_set_t       primitiveTableSet;
        primitive_array_table_set_t primitiveArrayTableSet;
        blob_array_table_set_t      blobArrayTableSet;
        reference_array_table_set_t referenceArrayTableSet;
    };
}  // namespace alex
