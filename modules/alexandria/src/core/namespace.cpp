#include "alexandria/core/namespace.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <format>
#include <regex>

namespace alex
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    Namespace::Namespace(Library& lib, const sql::row_id rid, std::string namespaceName) :
        library(&lib), id(rid), name(std::move(namespaceName))
    {
    }

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    Library& Namespace::getLibrary() noexcept { return *library; }

    const Library& Namespace::getLibrary() const noexcept { return *library; }

    sql::row_id Namespace::getId() const noexcept { return id; }

    const std::string& Namespace::getName() const noexcept { return name; }

    Type& Namespace::getType(const std::string& typeName)
    {
        const auto it = types.find(typeName);
        if (it == types.end())
            throw std::runtime_error(
              std::format(R"(Type with name "{}" does not exist in namespace "{}".)", typeName, name));
        return *it->second;
    }

    const Type& Namespace::getType(const std::string& typeName) const
    {
        const auto it = types.find(typeName);
        if (it == types.end())
            throw std::runtime_error(
              std::format(R"(Type with name "{}" does not exist in namespace "{}".)", typeName, name));
        return *it->second;
    }

    ////////////////////////////////////////////////////////////////
    // Types.
    ////////////////////////////////////////////////////////////////

    Type& Namespace::createType(const std::string& typeName, const bool instantiable)
    {
        if (types.contains(typeName))
            throw std::runtime_error(
              std::format(R"(A type with name "{}" already exists in namespace "{}".)", typeName, name));

        if (const std::regex regex("^[a-z][a-z0-9_]*$"); !std::regex_match(typeName, regex))
            throw std::runtime_error(std::format(
              R"(Cannot create type with name "{}". It does not match the regex "^[a-z][a-z0-9_]*$".)", typeName));

        auto type = std::make_unique<Type>(*this, typeName, instantiable);
        return *types.emplace(typeName, std::move(type)).first->second;
    }
}  // namespace alex
