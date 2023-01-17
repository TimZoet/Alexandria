#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <string>

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "alexandria/core/type.h"

namespace alex
{
    class Namespace
    {
    public:
        friend class Library;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        Namespace() = delete;

        Namespace(const Namespace&) = delete;

        Namespace(Namespace&&) noexcept = default;

        Namespace(Library& lib, sql::row_id rid, std::string namespaceName);

        ~Namespace() noexcept = default;

        Namespace& operator=(const Namespace&) = delete;

        Namespace& operator=(Namespace&&) noexcept = default;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] Library& getLibrary() noexcept;

        [[nodiscard]] const Library& getLibrary() const noexcept;

        /**
         * \brief Get internal type ID.
         * \return Type ID.
         */
        [[nodiscard]] sql::row_id getId() const noexcept;

        [[nodiscard]] const std::string& getName() const noexcept;

        [[nodiscard]] Type& getType(const std::string& typeName);

        [[nodiscard]] const Type& getType(const std::string& typeName) const;

        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Create a new type.
         * \param typeName Unique type name.
         * \param instantiable Whether instances of this type can be created, or if it is only to be used as a nested type.
         * If false, no tables are generated when committing this type.
         * \return Type.
         */
        Type& createType(const std::string& typeName, bool instantiable = true);

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Library.
         */
        Library* library = nullptr;

        /**
         * \brief Row ID.
         */
        sql::row_id id;

        /**
         * \brief Namespace name.
         */
        std::string name;

        /**
         * \brief Types.
         */
        TypeMap types;
    };
}  // namespace alex