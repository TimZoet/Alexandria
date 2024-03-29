#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <memory>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria-core/library.h"
#include "alexandria-core/namespace.h"
#include "alexandria-core/type.h"
#include "cppql/core/transaction.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "alexandria-basic-query/deleters/blob_array_deleter.h"
#include "alexandria-basic-query/deleters/primitive_array_deleter.h"
#include "alexandria-basic-query/deleters/reference_array_deleter.h"
#include "alexandria-basic-query/inserters/blob_array_inserter.h"
#include "alexandria-basic-query/inserters/primitive_array_inserter.h"
#include "alexandria-basic-query/inserters/reference_array_inserter.h"
#include "alexandria-basic-query/updaters/primitive_updater.h"

namespace alex
{
    /**
     * \brief
     * \tparam T TypeDescriptor.
     */
    template<typename T>
    class UpdateQuery
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        using type_descriptor_t          = T;
        using object_t                   = typename type_descriptor_t::object_t;
        using primitive_array_deleter_t  = PrimitiveArrayDeleter<type_descriptor_t>;
        using blob_array_deleter_t       = BlobArrayDeleter<type_descriptor_t>;
        using reference_array_deleter_t  = ReferenceArrayDeleter<type_descriptor_t>;
        using primitive_updater_t        = PrimitiveUpdater<type_descriptor_t>;
        using primitive_array_inserter_t = PrimitiveArrayInserter<type_descriptor_t>;
        using blob_array_inserter_t      = BlobArrayInserter<type_descriptor_t>;
        using reference_array_inserter_t = ReferenceArrayInserter<type_descriptor_t>;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        UpdateQuery() = delete;

        UpdateQuery(const UpdateQuery&) = delete;

        UpdateQuery(UpdateQuery&&) noexcept = delete;

        explicit UpdateQuery(type_descriptor_t desc) :
            descriptor(desc),
            uuidParam(std::make_unique<std::string>()),
            primitiveArrayDeleter(desc, *uuidParam),
            blobArrayDeleter(desc, *uuidParam),
            referenceArrayDeleter(desc, *uuidParam),
            primitiveUpdater(desc, *uuidParam),
            primitiveArrayInserter(desc),
            blobArrayInserter(desc),
            referenceArrayInserter(desc)
        {
        }

        virtual ~UpdateQuery() noexcept = default;

        UpdateQuery& operator=(const UpdateQuery&) = delete;

        UpdateQuery& operator=(UpdateQuery&&) noexcept = delete;

        ////////////////////////////////////////////////////////////////
        // Invoke.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Overwrite all data of the instance in the database.
         * \param instance Instance.
         * \return True if instance was updated, false if something failed.
         */
        virtual bool operator()(object_t& instance)
        {
            // Cannot update an object that does not have valid ID.
            if (!type_descriptor_t::uuid_member_t::template get(instance).valid())
                throw std::runtime_error("Cannot update instance. It does not have a valid UUID.");

            Type& type = descriptor.getType();
            auto& db   = type.getNamespace().getLibrary().getDatabase();
            try
            {
                // Update parameter.
                *uuidParam                = type_descriptor_t::uuid_member_t::template get(instance).getAsString();
                const std::string uuidstr = type_descriptor_t::uuid_member_t::template get(instance).getAsString();
                const auto        uuid    = sql::toStaticText(uuidstr);

                // Start transaction.
                sql::Transaction transaction(db, sql::Transaction::Type::Deferred);

                // Run all statements.
                primitiveArrayDeleter();
                blobArrayDeleter();
                referenceArrayDeleter();
                primitiveUpdater(instance);
                primitiveArrayInserter(instance, uuid);
                blobArrayInserter(instance, uuid);
                referenceArrayInserter(instance, uuid);

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
        primitive_array_deleter_t    primitiveArrayDeleter;
        blob_array_deleter_t         blobArrayDeleter;
        reference_array_deleter_t    referenceArrayDeleter;
        primitive_updater_t          primitiveUpdater;
        primitive_array_inserter_t   primitiveArrayInserter;
        blob_array_inserter_t        blobArrayInserter;
        reference_array_inserter_t   referenceArrayInserter;
    };
}  // namespace alex
