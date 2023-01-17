#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <concepts>
#include <functional>

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include "uuid.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "common/type_traits.h"
#include "cppql/core/database.h"

namespace alex
{
    class InstanceId
    {
    public:
        static constexpr uuids::uuid invalid_id = uuids::uuid{};

        InstanceId() = default;

        InstanceId(const InstanceId&) = default;

        InstanceId(InstanceId&&) noexcept = default;

        explicit InstanceId(const uuids::uuid iid) : id(iid) {}

        // TODO: Dereferencing might throw.
        explicit InstanceId(const std::string& iid) : id(*uuids::uuid::from_string(iid)) {}

        ~InstanceId() noexcept = default;

        ////////////////////////////////////////////////////////////////
        // Assignment operators.
        ////////////////////////////////////////////////////////////////

        InstanceId& operator=(const InstanceId&) = default;

        InstanceId& operator=(InstanceId&&) noexcept = default;

        InstanceId& operator=(const uuids::uuid iid) noexcept
        {
            id = iid;
            return *this;
        }

        InstanceId& operator=(const std::string& iid) noexcept
        {
            // TODO: What if empty.
            if (const auto i = uuids::uuid::from_string(iid); i.has_value()) id = *i;
            return *this;
        }

        void regenerate();

        void clear() noexcept { id = invalid_id; }

        ////////////////////////////////////////////////////////////////
        // Comparison operators.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] bool operator==(const InstanceId& rhs) const noexcept { return id == rhs.id; }

        [[nodiscard]] bool operator==(const uuids::uuid rhs) const noexcept { return id == rhs; }

        ////////////////////////////////////////////////////////////////
        // Conversion operators.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] explicit operator uuids::uuid() const noexcept { return id; }

        [[nodiscard]] explicit operator std::string() const { return getAsString(); }

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] uuids::uuid get() const noexcept { return id; }

        [[nodiscard]] std::string getAsString() const noexcept { return to_string(id); }

        [[nodiscard]] bool valid() const noexcept { return id != invalid_id; }

    private:
        uuids::uuid id = invalid_id;
    };

    template<typename T>
    concept is_instance_id = std::same_as<InstanceId, T>;

    template<auto M>
    concept is_instance_id_mp = is_instance_id<member_pointer_value_t<decltype(M)>>;

    // clang-format off
    template<typename T>
    concept has_instance_id = requires (T obj)
    {
        {obj.id} -> std::same_as<InstanceId&>;
    };
    // clang-format on

}  // namespace alex

template<>
struct std::hash<alex::InstanceId>
{
    std::size_t operator()(const alex::InstanceId& id) const noexcept { return std::hash<uuids::uuid>{}(id.get()); }
};  // namespace std
