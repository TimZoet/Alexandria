#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <type_traits>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "common/type_traits.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "alexandria/member_types/instance_id.h"

namespace alex
{
    /**
     * \brief Wrapper class that holds a reference to an object by storing its ID.
     * \tparam T Object type.
     */
    template<has_instance_id T>
    class Reference
    {
    public:
        /**
         * \brief Type of the object that is referenced.
         */
        using object_t = T;

        Reference() = default;

        Reference(const Reference& other) { *this = other; }

        Reference(Reference&& other) noexcept { *this = std::move(other); }

        explicit Reference(const InstanceId instanceId) : id(instanceId) {}

        explicit Reference(const object_t& instance) : id(instance.id) {}

        ~Reference() noexcept = default;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Returns whether this reference is empty.
         * \return True if empty.
         */
        [[nodiscard]] bool isNone() const noexcept { return !id.valid(); }

        /**
         * \brief Get the InstanceId of the referenced object. Will return an invalid ID if no object is referenced.
         * \return InstanceId.
         */
        [[nodiscard]] InstanceId getId() const noexcept { return id; }

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Reset this reference to point to no object.
         */
        void clear() noexcept { id.reset(); }

        /**
         * \brief Assign an object to this reference.
         * \param instance Object to assign.
         * \return *this.
         */
        Reference<object_t>& operator=(const object_t& instance)
        {
            if (!instance.id.valid()) throw std::runtime_error("Cannot assign an invalid object");
            id = instance.id;
            return *this;
        }

        /**
         * \brief Assign the object referenced by the other reference to this reference. If the other reference does not point to any object, this reference is cleared also.
         * \param ref Object reference to copy.
         * \return *this.
         */
        Reference<object_t>& operator=(const Reference<object_t>& ref) noexcept
        {
            id = ref.id;
            return *this;
        }

        /**
         * \brief Assign the object referenced by the other reference to this reference. Other reference is cleared. If the other reference does not point to any object, this reference is also cleared.
         * \param ref Object reference to copy.
         * \return *this.
         */
        Reference<object_t>& operator=(Reference<object_t>&& ref) noexcept
        {
            id = ref.id;
            ref.clear();
            return *this;
        }

        Reference<object_t>& operator=(std::string uuid) noexcept
        {
            id = std::move(uuid);
            return *this;
        }

    private:
        InstanceId id;
    };

    ////////////////////////////////////////////////////////////////
    // Type traits.
    ////////////////////////////////////////////////////////////////

    template<typename T>
    struct _is_reference : std::false_type
    {
    };

    template<typename T>
    struct _is_reference<Reference<T>> : std::true_type
    {
    };

    template<typename T>
    concept is_reference = _is_reference<T>::value;

    template<auto M>
    concept is_reference_mp = is_reference<member_pointer_value_t<decltype(M)>>;
}  // namespace alex
