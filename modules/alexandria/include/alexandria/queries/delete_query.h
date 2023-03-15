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
#include "alexandria/queries/deleters/primitive_deleter.h"

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

        using type_descriptor_t   = T;
        using object_t            = typename type_descriptor_t::object_t;
        using primitive_deleter_t = PrimitiveDeleter<type_descriptor_t>;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        DeleteQuery() = delete;

        DeleteQuery(const DeleteQuery&) = delete;

        DeleteQuery(DeleteQuery&&) noexcept = delete;

        explicit DeleteQuery(type_descriptor_t desc) :
            descriptor(desc), uuidParam(std::make_unique<std::string>()), primitiveDeleter(desc, *uuidParam)
        {
        }

        virtual ~DeleteQuery() noexcept = default;

        DeleteQuery& operator=(const DeleteQuery&) = delete;

        DeleteQuery& operator=(DeleteQuery&&) noexcept = delete;

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

        virtual bool operator()(const InstanceId& id)
        {
            try
            {
                // Update parameter.
                *uuidParam = id.getAsString();

                // Start transaction.
                Type&            type = descriptor.getType();
                auto&            db   = type.getNamespace().getLibrary().getDatabase();
                sql::Transaction transaction(db, sql::Transaction::Type::Deferred);

                primitiveDeleter();

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
    };
}  // namespace alex
