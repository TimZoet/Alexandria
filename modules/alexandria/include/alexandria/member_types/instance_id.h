#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <concepts>
#include <cstdint>
#include <functional>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "common/type_traits.h"
#include "cppql-core/database.h"

namespace alex
{
    class InstanceId
    {
    public:
        using type = sql::row_id;

        static constexpr type invalid_id = 0;

        InstanceId() = default;

        InstanceId(const InstanceId&) = default;

        InstanceId(InstanceId&&) = default;

        InstanceId(const type iid) : id(iid) {}

        ~InstanceId() = default;

        ////////////////////////////////////////////////////////////////
        // Assignment operators.
        ////////////////////////////////////////////////////////////////

        InstanceId& operator=(const InstanceId&) = default;

        InstanceId& operator=(InstanceId&&) = default;

        InstanceId& operator=(const type iid) noexcept
        {
            id = iid;
            return *this;
        }

        ////////////////////////////////////////////////////////////////
        // Comparison operators.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] bool operator==(const InstanceId& rhs) const noexcept { return id == rhs.id; }

        [[nodiscard]] bool operator==(const type rhs) const noexcept { return id == rhs; }

        ////////////////////////////////////////////////////////////////
        // Conversion operators.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] operator type() const noexcept { return id; }

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] type get() const noexcept { return id; }

        [[nodiscard]] bool valid() const noexcept { return id != invalid_id; }

    private:
        type id = 0;
    };

    template<auto M>
    concept is_instance_id_mp = std::same_as<InstanceId, member_pointer_value_t<decltype(M)>>;
}  // namespace alex

namespace std
{
    template<>
    struct hash<alex::InstanceId>
    {
        std::size_t operator()(const alex::InstanceId& id) const noexcept
        {
            return std::hash<alex::InstanceId::type>{}(id.get());
        }
    };
}  // namespace std