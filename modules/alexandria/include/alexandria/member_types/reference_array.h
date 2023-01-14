#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <vector>

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
     * \brief Wrapper class that holds a list of references to an object by storing their IDs.
     * \tparam T Object type.
     */
    template<typename T>
    class ReferenceArray
    {
    public:
        /**
         * \brief Type of the object that is referenced.
         */
        using object_t = T;

        using value_t = std::vector<InstanceId>;

        ReferenceArray() = default;

        ~ReferenceArray() = default;

        ReferenceArray(const ReferenceArray&) = default;

        ReferenceArray(ReferenceArray&&) noexcept = default;

        ReferenceArray& operator=(const ReferenceArray&) = default;

        ReferenceArray& operator=(ReferenceArray&&) = default;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] value_t& get() noexcept { return ids; }

        [[nodiscard]] const value_t& get() const noexcept { return ids; }

        ////////////////////////////////////////////////////////////////
        // Modifiers.
        ////////////////////////////////////////////////////////////////

        void add(const object_t& instance)
        {
            if (!instance.id.valid()) throw std::runtime_error("Instance was not yet inserted");
            ids.push_back(instance.id);
        }

        bool remove(const object_t& instance)
        {
            if (!instance.id.valid()) throw std::runtime_error("Instance was not yet inserted");
            auto it = std::find(ids.begin(), ids.end(), instance.id);
            if (it == ids.end()) return false;
            ids.erase(it);
            return true;
        }

        bool contains(const object_t& instance) const noexcept
        {
            return std::find(ids.begin(), ids.end(), instance.id) != ids.end();
        }

    private:
        value_t ids;
    };

    ////////////////////////////////////////////////////////////////
    // Type traits.
    ////////////////////////////////////////////////////////////////

    template<typename T>
    struct _is_reference_array : std::false_type
    {
    };

    template<typename T>
    struct _is_reference_array<ReferenceArray<T>> : std::true_type
    {
    };

    template<typename T>
    concept is_reference_array = _is_reference_array<T>::value;

    template<auto M>
    concept is_reference_array_mp = is_reference_array<member_pointer_value_t<decltype(M)>>;
}  // namespace alex
