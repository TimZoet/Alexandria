#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <format>
#include <type_traits>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "common/type_traits.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "alexandria-query/utils.h"
#include "alexandria-query/types/member_extractor.h"

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

        void operator()(object_t& instance, const sql::StaticText& uuid)
        {
            const auto getter = [&]<is_member M>(M) {
                if constexpr (M::is_primitive_blob || M::is_blob)
                {
                    if constexpr (explicitly_convertible_to<decltype(M::template get(instance)), sql::StaticBlob>)
                        return static_cast<sql::StaticBlob>(M::template get(instance));
                    else if constexpr (explicitly_convertible_to<decltype(M::template get(instance)),
                                                                 sql::TransientBlob>)
                        return static_cast<sql::TransientBlob>(M::template get(instance));
                    else
                        return static_cast<sql::Blob>(M::template get(instance));
                }
                else if constexpr (M::is_reference)
                    return sql::toText(M::template get(instance).getId().getAsString());
                else if constexpr (M::is_primitive)
                    return M::template get(instance);
                else if constexpr (M::is_string)
                    return sql::toText(M::template get(instance));
                else
                    constexpr_static_assert();
            };

            const auto f = [&]<is_member M, is_member... Ms>(std::tuple<M, Ms...>)
            {
                try
                {
                    // Insert nullptr for autoincrement rowid, retrieve member values from instance for other columns.
                    statement(nullptr, uuid, getter(Ms())...);
                }
                catch (const sql::SqliteError& e)
                {
                    if (e.getExtendedErrorCode() == 787)
                        throw std::runtime_error(std::format(
                          "Failed to insert instance into main instance table. Most likely cause: instance has a "
                          "reference property that references an object that no longer exists. Internal error message: "
                          "\"{}\"",
                          e.what()));

                    throw std::runtime_error(std::format("Failed to insert instance into main instance table. Most "
                                                         "likely cause unknown. Internal error message: \"{}\"",
                                                         e.what()));
                }
                catch (const std::exception& e)
                {
                    throw std::runtime_error(std::format("Failed to insert instance into main instance table. Most "
                                                         "likely cause unknown. Internal error message: \"{}\"",
                                                         e.what()));
                }
            };

            f(members_t{});

            statement.clearBindings();
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
