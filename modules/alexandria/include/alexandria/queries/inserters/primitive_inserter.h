#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <type_traits>

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "alexandria/queries/utils.h"
#include "alexandria/queries/types/member_extractor.h"

namespace alex
{
    /**
     * \brief The PrimitiveInserter handles the insertion of all columns of the instance table. This includes not
     * just integers and floats, but also the UUID and single string, blob and reference columns.
     * \tparam T TypeDescriptor.
     */
    template<typename T>
    class PrimitiveInserter
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
         * \brief Insert query type.
         */
        using query_t = std::remove_cvref_t<decltype(std::declval<table_t>().insert())>;

        /**
         * \brief Insert statement type.
         */
        using statement_t = std::remove_cvref_t<decltype(std::declval<query_t>().compile())>;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        PrimitiveInserter() = delete;

        explicit PrimitiveInserter(const type_descriptor_t& desc) : statement(compile(desc)) {}

        PrimitiveInserter(const PrimitiveInserter&) = delete;

        PrimitiveInserter(PrimitiveInserter&&) = default;

        ~PrimitiveInserter() noexcept = default;

        PrimitiveInserter& operator=(const PrimitiveInserter&) = delete;

        PrimitiveInserter& operator=(PrimitiveInserter&&) = default;

        ////////////////////////////////////////////////////////////////
        // Invoke.
        ////////////////////////////////////////////////////////////////

        void operator()(object_t& instance)
        {
            // TODO: This getter needs to be moved out of here.
            const auto getter = [&]<typename M>(M) {
                // TODO: All these toText copy data. Might not always be necessary.
                if constexpr (M::is_instance_id)
                    return sql::toText(M::template get(instance).getAsString());
                else if constexpr (M::is_blob || M::is_primitive_blob)
                    return M::template get(instance).getStaticBlob();
                else if constexpr (M::is_reference)
                    // TODO: What if no object was assigned? Turn return type into std::optional and rely on cppql to insert nullptr?
                    return sql::toText(M::template get(instance).getId().getAsString());
                else if constexpr (M::is_primitive)
                    return M::template get(instance);
                else if constexpr (M::is_string)
                    return sql::toText(M::template get(instance));
                else
                    constexpr_static_assert();
            };

            const auto f = [&]<typename... Ms>(std::tuple<Ms...>) {
                // Insert nullptr for autoincrement rowid, retrieve member values from instance for other columns.
                statement(nullptr, getter(Ms())...);
            };

            f(members_t{});
        }

    private:
        [[nodiscard]] static statement_t compile(const type_descriptor_t& desc)
        {
            const auto table = table_t(desc.getType().getInstanceTable());
            return table.insert().compile();
        }

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        statement_t statement;
    };
}  // namespace alex
