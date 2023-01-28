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
        // Invoke.
        ////////////////////////////////////////////////////////////////

        auto& getInstanceTable() noexcept { return primitiveTableSet.get(); }

        template<size_t I>
            requires(I < primitive_array_table_set_t::size)
        auto& getPrimitiveArrayTable() noexcept
        {
            return primitiveArrayTableSet.template get<I>();
        }

        template<size_t I>
            requires(I < blob_array_table_set_t::size)
        auto& getBlobArrayTable() noexcept
        {
            return blobArrayTableSet.template get<I>();
        }

        template<size_t I>
            requires(I < reference_array_table_set_t::size)
        auto& getReferenceArrayTable() noexcept
        {
            return referenceArrayTableSet.template get<I>();
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
