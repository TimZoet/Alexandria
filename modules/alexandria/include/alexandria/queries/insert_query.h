#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "common/type_traits.h"
#include "cppql/core/transaction.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "alexandria/core/library.h"
#include "alexandria/core/namespace.h"
#include "alexandria/core/type.h"
#include "alexandria/core/type_descriptor.h"
#include "alexandria/queries/utils.h"
#include "alexandria/queries/inserters/blob_array_inserter.h"
#include "alexandria/queries/inserters/primitive_array_inserter.h"
#include "alexandria/queries/inserters/primitive_inserter.h"
#include "alexandria/queries/inserters/reference_array_inserter.h"

namespace alex
{
    /**
     * \brief 
     * \tparam T TypeDescriptor.
     */
    template<typename T>
    class InsertQuery
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        using type_descriptor_t    = T;
        using object_t             = typename type_descriptor_t::object_t;
        using primitive_inserter_t = PrimitiveInserter<type_descriptor_t>;
        using primitive_array_members_t =
          detail::extract_primitive_array_members_t<typename type_descriptor_t::user_members_t>;
        using blob_array_members_t = detail::extract_blob_array_members_t<typename type_descriptor_t::user_members_t>;
        using reference_array_members_t =
          detail::extract_reference_array_members_t<typename type_descriptor_t::user_members_t>;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        explicit InsertQuery(type_descriptor_t desc) : descriptor(desc), primitiveInserter(desc) {}

        ////////////////////////////////////////////////////////////////
        // ...
        ////////////////////////////////////////////////////////////////

        void operator()(object_t& instance)
        {
            // Cannot insert an object that already has a valid ID.
            if (type_descriptor_t::uuid_member_t::template get(instance).valid())
                throw std::runtime_error("Cannot insert instance. It already has a valid UUID.");

            Type& type = descriptor.getType();
            auto& db   = type.getNamespace().getLibrary().getDatabase();
            try
            {
                // Generate UUID.
                type_descriptor_t::uuid_member_t::template get(instance).regenerate();

                sql::Transaction transaction(db, sql::Transaction::Type::Deferred);

                primitiveInserter(instance);

                transaction.commit();
            }
            catch (...)
            {
                // Transaction failed (or something else went wrong). Reset UUID.
                type_descriptor_t::uuid_member_t::template get(instance).clear();
                throw;
            }
        }

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        type_descriptor_t    descriptor;
        primitive_inserter_t primitiveInserter;
    };
}  // namespace alex