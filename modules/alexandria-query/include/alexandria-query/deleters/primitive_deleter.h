#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <type_traits>

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "alexandria-query/utils.h"
#include "alexandria-query/types/member_extractor.h"

namespace alex
{
    /**
     * \brief The PrimitiveDeleter handles the removal of rows from the instance table.
     * \tparam T TypeDescriptor.
     */
    template<typename T>
    class PrimitiveDeleter
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

        /**
         * \brief Delete query type.
         */
        using query_t = std::remove_cvref_t<decltype(std::declval<table_t>().del())>;

        /**
         * \brief Delete statement type.
         */
        using statement_t = std::remove_cvref_t<decltype(std::declval<query_t>().compile())>;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        PrimitiveDeleter() = delete;

        PrimitiveDeleter(const type_descriptor_t& desc, std::string& uuidParam) : statement(compile(desc, uuidParam)) {}

        PrimitiveDeleter(const PrimitiveDeleter&) = delete;

        PrimitiveDeleter(PrimitiveDeleter&&) = default;

        ~PrimitiveDeleter() noexcept = default;

        PrimitiveDeleter& operator=(const PrimitiveDeleter&) = delete;

        PrimitiveDeleter& operator=(PrimitiveDeleter&&) = default;

        ////////////////////////////////////////////////////////////////
        // Invoke.
        ////////////////////////////////////////////////////////////////

        void operator()()
        {
            statement.bind(sql::BindParameters::Dynamic)();
            statement.clearBindings();
        }

    private:
        [[nodiscard]] static statement_t compile(const type_descriptor_t& desc, std::string& uuidParam)
        {
            const auto table = table_t(desc.getType().getInstanceTable());
            return table.del().where(sql::like(table.template col<1>(), &uuidParam)).compile();
        }

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        statement_t statement;
    };
}  // namespace alex
