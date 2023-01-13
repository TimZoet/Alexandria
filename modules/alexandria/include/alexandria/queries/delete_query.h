#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <memory>
#include <type_traits>

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "alexandria/core/library.h"
#include "alexandria/core/namespace.h"
#include "alexandria/core/type.h"
#include "alexandria/core/type_descriptor.h"
#include "alexandria/queries/utils.h"
#include "alexandria/queries/deleters/blob_array_deleter.h"
#include "alexandria/queries/deleters/primitive_array_deleter.h"
#include "alexandria/queries/deleters/primitive_deleter.h"
#include "alexandria/queries/deleters/reference_array_deleter.h"

namespace alex
{
    /**
     * \brief
     * \tparam T TypeDescriptor.
     */
    template<typename T>
    class DeleteQuery
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        using type_descriptor_t         = T;
        using object_t                  = typename type_descriptor_t::object_t;
        using primitive_deleter_t       = PrimitiveDeleter<type_descriptor_t>;
        using primitive_array_deleter_t = PrimitiveArrayDeleter<type_descriptor_t>;
        using blob_array_deleter_t      = BlobArrayDeleter<type_descriptor_t>;
        using reference_array_deleter_t = ReferenceArrayDeleter<type_descriptor_t>;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        explicit DeleteQuery(type_descriptor_t desc) :
            descriptor(desc),
            uuidParam(std::make_unique<std::string>()),
            primitiveDeleter(desc, *uuidParam),
            primitiveArrayDeleter(desc, *uuidParam),
            blobArrayDeleter(desc, *uuidParam),
            referenceArrayDeleter(desc, *uuidParam)
        {
        }

        ////////////////////////////////////////////////////////////////
        // Invoke.
        ////////////////////////////////////////////////////////////////

        void operator()(const object_t& instance)
        {
            // Cannot delete an object that does not have a valid ID.
            if (!type_descriptor_t::uuid_member_t::template get(instance).valid())
                throw std::runtime_error("Cannot delete instance. It does not have a valid UUID.");

            try
            {
                // Update parameter.
                *uuidParam            = type_descriptor_t::uuid_member_t::template get(instance).getAsString();

                // Start transaction.
                Type&            type = descriptor.getType();
                auto&            db   = type.getNamespace().getLibrary().getDatabase();
                sql::Transaction transaction(db, sql::Transaction::Type::Deferred);

                // Run all statements.
                primitiveDeleter();
                primitiveArrayDeleter();
                blobArrayDeleter();
                referenceArrayDeleter();

                // TODO: What if the object didn't exist in the first place? Should that throw or return some kind of error code?
                transaction.commit();
            }
            catch (...)
            {
                // Transaction failed (or something else went wrong).
                throw;
            }
        }

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        type_descriptor_t            descriptor;
        std::unique_ptr<std::string> uuidParam;
        primitive_deleter_t          primitiveDeleter;
        primitive_array_deleter_t    primitiveArrayDeleter;
        blob_array_deleter_t         blobArrayDeleter;
        reference_array_deleter_t    referenceArrayDeleter;
    };
}  // namespace alex
