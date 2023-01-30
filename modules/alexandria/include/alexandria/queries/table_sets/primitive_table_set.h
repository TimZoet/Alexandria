#pragma once

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "alexandria/queries/utils.h"
#include "alexandria/queries/types/member_extractor.h"

namespace alex
{
    /**
     * \brief The PrimitiveTableSet holds the instance table.
     * \tparam T TypeDescriptor.
     */
    template<typename T>
    class PrimitiveTableSet
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief TypeDescriptor.
         */
        using type_descriptor_t = T;

        /**
         * \brief Object type.
         */
        using object_t = typename type_descriptor_t::object_t;

        /**
         * \brief Concatenation of the UUID member and all primitive members.
         */
        using members_t = detail::extract_primitive_members_t<typename type_descriptor_t::members_t>;

        /**
         * \brief TypedTable for the instance table.
         */
        using table_t = detail::primitive_table_t<members_t>;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        PrimitiveTableSet() = delete;

        explicit PrimitiveTableSet(const type_descriptor_t& desc) : table(compile(desc)) {}

        PrimitiveTableSet(const PrimitiveTableSet&) = delete;

        PrimitiveTableSet(PrimitiveTableSet&&) = default;

        ~PrimitiveTableSet() noexcept = default;

        PrimitiveTableSet& operator=(const PrimitiveTableSet&) = delete;

        PrimitiveTableSet& operator=(PrimitiveTableSet&&) = default;

        ////////////////////////////////////////////////////////////////
        // Invoke.
        ////////////////////////////////////////////////////////////////

        auto& get() noexcept { return table; }

    private:
        [[nodiscard]] static table_t compile(const type_descriptor_t& desc)
        {
            return table_t(desc.getType().getInstanceTable());
        }

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        table_t table;
    };
}  // namespace alex
