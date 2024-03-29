#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <format>
#include <memory>
#include <string>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "cppql/include_all.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "alexandria-core/properties/instance_id.h"

namespace alex
{
    class Library;
    class Namespace;
    class Property;
    class PropertyLayout;
    class Type;
    class TypeLayout;

    using LibraryPtr        = std::unique_ptr<Library>;
    using NamespacePtr      = std::unique_ptr<Namespace>;
    using PropertyPtr       = std::unique_ptr<Property>;
    using PropertyLayoutPtr = std::unique_ptr<PropertyLayout>;
    using TypePtr           = std::unique_ptr<Type>;
    using TypeLayoutPtr     = std::unique_ptr<TypeLayout>;
    using NamespaceMap      = std::map<std::string, NamespacePtr>;
    using TypeMap           = std::map<std::string, TypePtr>;

    struct NamespaceRow
    {
        sql::row_id id;
        std::string name;

        [[nodiscard]] bool operator==(const NamespaceRow& rhs) const noexcept
        {
            return id == rhs.id && name == rhs.name;
        }

        friend std::ostream& operator<<(std::ostream& out, const NamespaceRow& ns)
        {
            return out << std::format("Namespace(id={}, name={})", ns.id, ns.name);
        }
    };

    struct TypeRow
    {
        sql::row_id id;
        sql::row_id nameSpace;
        std::string name;
        int32_t     isInstance;

        [[nodiscard]] bool operator==(const TypeRow& rhs) const noexcept
        {
            return id == rhs.id && nameSpace == rhs.nameSpace && name == rhs.name && isInstance == rhs.isInstance;
        }

        friend std::ostream& operator<<(std::ostream& out, const TypeRow& type)
        {
            return out << std::format("Type(id={}, namespace={}, name={}, isInstance={})",
                                      type.id,
                                      type.nameSpace,
                                      type.name,
                                      type.isInstance);
        }
    };

    struct PropertyRow
    {
        sql::row_id id;
        sql::row_id type;
        std::string name;
        std::string dataType;
        sql::row_id referenceType;
        int32_t     isArray;
        int32_t     isBlob;

        [[nodiscard]] bool operator==(const PropertyRow& rhs) const noexcept
        {
            return id == rhs.id && type == rhs.type && name == rhs.name && dataType == rhs.dataType &&
                   referenceType == rhs.referenceType && isArray == rhs.isArray && isBlob == rhs.isBlob;
        }

        friend std::ostream& operator<<(std::ostream& out, const PropertyRow& prop)
        {
            return out << std::format(
                     "Property(id={}, type={}, name={}, dataType={}, referenceType={}, isArray={}, isBlob={})",
                     prop.id,
                     prop.type,
                     prop.name,
                     prop.dataType,
                     prop.referenceType,
                     prop.isArray,
                     prop.isBlob);
        }
    };

    struct TableRow
    {
        sql::row_id id;
        sql::row_id type;
        std::string name;
        std::string kind;

        [[nodiscard]] bool operator==(const TableRow& rhs) const noexcept
        {
            return id == rhs.id && type == rhs.type && name == rhs.name && kind == rhs.kind;
        }

        friend std::ostream& operator<<(std::ostream& out, const TableRow& table)
        {
            return out << std::format(
                     "Generated Table(id={}, type={}, name={}, kind={})", table.id, table.type, table.name, table.kind);
        }
    };

    using NamespaceTable = sql::TypedTable<decltype(NamespaceRow::id), decltype(NamespaceRow::name)>;

    using TypeTable = sql::TypedTable<decltype(TypeRow::id),
                                      decltype(TypeRow::nameSpace),
                                      decltype(TypeRow::name),
                                      decltype(TypeRow::isInstance)>;

    using PropertyTable = sql::TypedTable<decltype(PropertyRow::id),
                                          decltype(PropertyRow::type),
                                          decltype(PropertyRow::name),
                                          decltype(PropertyRow::dataType),
                                          decltype(PropertyRow::referenceType),
                                          decltype(PropertyRow::isBlob),
                                          decltype(PropertyRow::isArray)>;

    using GeneratedTablesTable = sql::
      TypedTable<decltype(TableRow::id), decltype(TableRow::type), decltype(TableRow::name), decltype(TableRow::kind)>;

    using NamespaceTableInsert = std::remove_cvref_t<decltype(std::declval<NamespaceTable>().insert().compile())>;

    using TypeTableInsert = std::remove_cvref_t<decltype(std::declval<TypeTable>().insert().compile())>;

    using GeneratedTablesInsert =
      std::remove_cvref_t<decltype(std::declval<GeneratedTablesTable>().insert().compile())>;
}  // namespace alex
