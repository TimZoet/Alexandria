#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <memory>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "alexandria-core/core/library.h"
#include "alexandria-core/core/namespace.h"
#include "alexandria-core/core/type.h"
#include "cppql/core/transaction.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "alexandria-query/queries/utils.h"
#include "alexandria-query/queries/getters/blob_array_getter.h"
#include "alexandria-query/queries/getters/primitive_array_getter.h"
#include "alexandria-query/queries/getters/primitive_getter.h"
#include "alexandria-query/queries/getters/reference_array_getter.h"

namespace alex
{
    /**
     * \brief
     * \tparam T TypeDescriptor.
     */
    template<typename T>
    class GetQuery
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        using type_descriptor_t        = T;
        using object_t                 = typename type_descriptor_t::object_t;
        using primitive_getter_t       = PrimitiveGetter<type_descriptor_t>;
        using primitive_array_getter_t = PrimitiveArrayGetter<type_descriptor_t>;
        using blob_array_getter_t      = BlobArrayGetter<type_descriptor_t>;
        using reference_array_getter_t = ReferenceArrayGetter<type_descriptor_t>;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        GetQuery() = delete;

        GetQuery(const GetQuery&) = delete;

        GetQuery(GetQuery&&) noexcept = delete;

        explicit GetQuery(type_descriptor_t desc) :
            descriptor(desc),
            uuidParam(std::make_unique<std::string>()),
            primitiveGetter(desc, *uuidParam),
            primitiveArrayGetter(desc, *uuidParam),
            blobArrayGetter(desc, *uuidParam),
            referenceArrayGetter(desc, *uuidParam)
        {
        }

        virtual ~GetQuery() noexcept = default;

        GetQuery& operator=(const GetQuery&) = delete;

        GetQuery& operator=(GetQuery&&) noexcept = delete;

        ////////////////////////////////////////////////////////////////
        // Invoke.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] object_t operator()(const std::string& uuid)
        {
            object_t instance{};
            type_descriptor_t::uuid_member_t::template get(instance) = *uuids::uuid::from_string(uuid);
            (*this)(instance);
            return instance;
        }

        [[nodiscard]] object_t operator()(const uuids::uuid& uuid)
        {
            object_t instance{};
            type_descriptor_t::uuid_member_t::template get(instance) = uuid;
            (*this)(instance);
            return instance;
        }

        [[nodiscard]] object_t operator()(const InstanceId& uuid)
        {
            object_t instance{};
            type_descriptor_t::uuid_member_t::template get(instance) = uuid;
            (*this)(instance);
            return instance;
        }

        virtual void operator()(object_t& instance)
        {
            // Cannot retrieve an object without a valid ID.
            if (!type_descriptor_t::uuid_member_t::template get(instance).valid())
                throw std::runtime_error("Cannot retrieve instance. It does not have a valid UUID.");

            Type& type = descriptor.getType();
            auto& db   = type.getNamespace().getLibrary().getDatabase();
            try
            {
                // Update parameter.
                *uuidParam = type_descriptor_t::uuid_member_t::template get(instance).getAsString();

                // Start transaction.
                sql::Transaction transaction(db, sql::Transaction::Type::Deferred);

                // Run all statements.
                primitiveGetter(instance);
                primitiveArrayGetter(instance);
                blobArrayGetter(instance);
                referenceArrayGetter(instance);

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
        primitive_getter_t           primitiveGetter;
        primitive_array_getter_t     primitiveArrayGetter;
        blob_array_getter_t          blobArrayGetter;
        reference_array_getter_t     referenceArrayGetter;
    };
}  // namespace alex
