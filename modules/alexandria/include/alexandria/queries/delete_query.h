#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <memory>

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "alexandria/core/library.h"
#include "alexandria/core/namespace.h"
#include "alexandria/core/type.h"
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

        /**
         * \brief Delete instance and all its data from the database. If instance was successfully deleted, its identifier is reset.
         * \param instance Instance.
         * \return True if instance was deleted, false if something failed.
         */
        bool operator()(object_t& instance)
        {
            // Cannot delete an object that does not have a valid ID.
            if (!type_descriptor_t::uuid_member_t::template get(instance).valid())
                throw std::runtime_error("Cannot delete instance. It does not have a valid UUID.");

            const bool deleted = this->operator()(type_descriptor_t::uuid_member_t::template get(instance));

            // Reset ID only upon success.
            if (deleted) type_descriptor_t::uuid_member_t::template get(instance).reset();

            return deleted;
        }

        bool operator()(const InstanceId& id)
        {
            try
            {
                // Update parameter.
                *uuidParam = id.getAsString();

                // Start transaction.
                Type&            type = descriptor.getType();
                auto&            db   = type.getNamespace().getLibrary().getDatabase();
                sql::Transaction transaction(db, sql::Transaction::Type::Deferred);

                // TODO: Hold up. Why do we need to run any array delete statements? Those all have FKs with cascade...
                // Run all statements.
                primitiveDeleter();
                primitiveArrayDeleter();
                blobArrayDeleter();
                referenceArrayDeleter();

                transaction.commit();

                return db.getChanges() > 0;
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
