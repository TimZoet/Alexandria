#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <functional>
#include <memory>
#include <string>
#include <vector>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "cppql-core/database.h"
#include "cppql-typed/typed_table.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "alexandria/property.h"

namespace alex
{
    class Library;
    class Type;
    using TypePtr      = std::unique_ptr<Type>;
    using PropertyList = std::vector<PropertyPtr>;

    class Type
    {
    public:
        friend class Library;
        friend class Property;

        Type() = delete;

        Type(const Type&) = delete;

        Type(Type&&) = default;

        explicit Type(std::string typeName);

        ~Type() = default;

        Type& operator=(const Type&) = delete;

        Type& operator=(Type&&) = default;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Returns whether this type was committed to the database.
         * \return True if committed.
         */
        [[nodiscard]] bool isCommitted() const noexcept;

        /**
         * \brief Get internal type ID.
         * \return Type ID.
         */
        [[nodiscard]] sql::row_id getId() const noexcept;

        /**
         * \brief Get type name.
         * \return Type name.
         */
        [[nodiscard]] const std::string& getName() const noexcept;

        /**
         * \brief Get the list of all top-level properties added to this type.
         * \return Property list.
         */
        [[nodiscard]] const PropertyList& getProperties() const noexcept;

        /**
         * \brief Get the list of all properties that match the predicate. Will also traverse all nested properties in depth first order.
         * \param predicate Function to filter properties by.
         * \return List of all properties that match predicate.
         */
        [[nodiscard]] std::vector<Property*> getProperties(const std::function<bool(const Property&)>& predicate) const;

        /**
         * \brief Get the list of all properties that precede the given property in the hierarchy.
         * \param prop Property.
         * \return List of properties. Does not include property parameter.
         */
        [[nodiscard]] std::vector<Property*> getPropertyParents(const Property& prop) const;

        /**
         * \brief Calculate the name of the table that would be generated for the given property. Does not perform any validity checks when it comes to e.g. being a member of this type or data types.
         * \param parents Parents.
         * \param prop Property.
         * \return Full table name.
         */
        [[nodiscard]] std::string resolveReferenceTableName(const std::vector<Property*>& parents,
                                                            const Property&               prop) const;

        [[nodiscard]] const std::vector<Property*>& getReferences() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Properties.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Create a new property with primitive data type.
         * \param propName Property name.
         * \param dataType Data type.
         * \return New property.
         */
        Property& createPrimitiveProperty(const std::string& propName, DataType dataType);

        /**
         * \brief Create a new property with string data type.
         * \param propName Property name.
         * \return New property.
         */
        Property& createStringProperty(const std::string& propName);

        /**
         * \brief Create a new property with blob data type.
         * \param propName Property name.
         * \return New property.
         */
        Property& createBlobProperty(const std::string& propName);

        /**
         * \brief Create a new reference property.
         * \param propName Property name.
         * \param refType Other type to reference.
         * \return New property.
         */
        Property& createReferenceProperty(const std::string& propName, Type& refType);

        /**
         * \brief Create a new member type property.
         * \param propName Property name.
         * \param memberType Other type to use as member.
         * \return New property.
         */
        Property& createTypeProperty(const std::string& propName, Type& memberType);

        /**
         * \brief Create a new array property with primitive data type.
         * \param propName Property name.
         * \param dataType Data type.
         * \return New property.
         */
        Property& createPrimitiveArrayProperty(const std::string& propName, DataType dataType);

        /**
         * \brief Create a new array property with string data type.
         * \param propName Property name.
         * \return New property.
         */
        Property& createStringArrayProperty(const std::string& propName);

        /**
         * \brief Create a new array property with blob data type.
         * \param propName Property name.
         * \return New property.
         */
        Property& createBlobArrayProperty(const std::string& propName);

        /**
         * \brief Create a new reference array property.
         * \param propName Property name.
         * \param refType Other type to reference.
         * \return New property.
         */
        Property& createReferenceArrayProperty(const std::string& propName, Type& refType);

        /**
         * \brief Create a new primitive blob array property.
         * \param propName Property name.
         * \param dataType Data type.
         * \return New property.
         */
        Property& createPrimitiveBlobProperty(const std::string& propName, DataType dataType);

    private:
        ////////////////////////////////////////////////////////////////
        // Private methods.
        ////////////////////////////////////////////////////////////////

        Property& createProperty(
          const std::string& propName, DataType dataType, Type* refType, bool isReference, bool isArray, bool isBlob);

        void addProperty(PropertyPtr prop);

        void addReference(Property& prop);

        /**
         * \brief Commit type to the database. Also commits all dependencies.
         * \param db Sqlite database.
         * \param typeTable Type table.
         * \param propsTable Property table.
         * \param memTable Member table.
         */
        void commit(sql::Database& db, TypeTable& typeTable, PropertyTable& propsTable, MemberTable& memTable);

        /**
         * \brief Generate columns for instance table and create reference tables.
         * \param db Sqlite database.
         * \param table Instance table.
         * \param prefix Concatenation of all parent property names.
         */
        void generateTables(sql::Database& db, sql::Table& table, const std::string& prefix) const;

        /**
         * \brief Resolve references.
         * \param db Sqlite database.
         * \param table Instance table.
         * \param prefix Concatenation of all parent property names.
         */
        void resolve(sql::Database& db, sql::Table& table, const std::string& prefix) const;

        /**
         * \brief Row ID.
         */
        sql::row_id id;

        /**
         * \brief Type name.
         */
        std::string name;

        /**
         * \brief Properties.
         */
        PropertyList properties;

        /**
         * \brief Properties that reference this type.
         */
        std::vector<Property*> references;
    };
}  // namespace alex