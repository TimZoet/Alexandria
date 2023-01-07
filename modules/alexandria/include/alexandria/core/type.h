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

#include "cppql/include_all.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "alexandria/core/fwd.h"
#include "alexandria/core/property.h"

namespace alex
{
    class Type
    {
    public:
        friend class Library;
        friend class Property;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        Type() = delete;

        Type(const Type&) = delete;

        Type(Type&&) = default;

        Type(Namespace& ns, std::string typeName);

        Type(sql::row_id rid, Namespace& ns, std::string typeName, bool isInst);

        ~Type() = default;

        Type& operator=(const Type&) = delete;

        Type& operator=(Type&&) = default;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Get namespace this type is in.
         * \return Namespace.
         */
        [[nodiscard]] Namespace& getNamespace() noexcept;

        /**
         * \brief Get namespace this type is in.
         * \return Namespace.
         */
        [[nodiscard]] const Namespace& getNamespace() const noexcept;

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

        [[nodiscard]] std::string getInstanceTableName() const noexcept;

        [[nodiscard]] sql::Table& getInstanceTable() const ;

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
        //[[nodiscard]] std::vector<Property*> getProperties(const std::function<bool(const Property&)>& predicate) const;

        /**
         * \brief Get the list of all properties that precede the given property in the hierarchy.
         * \param prop Property.
         * \return List of properties. Does not include property parameter.
         */
        //[[nodiscard]] std::vector<Property*> getPropertyParents(const Property& prop) const;

        /**
         * \brief Calculate the name of the table that would be generated for the given property. Does not perform any validity checks when it comes to e.g. being a member of this type or data types.
         * \param parents Parents.
         * \param prop Property.
         * \return Full table name.
         */
        [[nodiscard]] std::string resolveReferenceTableName(const std::vector<Property*>& parents,
                                                            const Property&               prop) const;

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
         * \brief Create a new array property with primitive data type.
         * \param propName Property name.
         * \param dataType Data type.
         * \return New property.
         */
        Property& createPrimitiveArrayProperty(const std::string& propName, DataType dataType);

        /**
         * \brief Create a new primitive blob array property.
         * \param propName Property name.
         * \param dataType Data type.
         * \return New property.
         */
        Property& createPrimitiveBlobProperty(const std::string& propName, DataType dataType);

        /**
         * \brief Create a new property with string data type.
         * \param propName Property name.
         * \return New property.
         */
        Property& createStringProperty(const std::string& propName);

        /**
         * \brief Create a new array property with string data type.
         * \param propName Property name.
         * \return New property.
         */
        Property& createStringArrayProperty(const std::string& propName);

        /**
         * \brief Create a new property with blob data type.
         * \param propName Property name.
         * \return New property.
         */
        Property& createBlobProperty(const std::string& propName);

        /**
         * \brief Create a new array property with blob data type.
         * \param propName Property name.
         * \return New property.
         */
        Property& createBlobArrayProperty(const std::string& propName);

        /**
         * \brief Create a new reference property.
         * \param propName Property name.
         * \param refType Other type to reference.
         * \return New property.
         */
        Property& createReferenceProperty(const std::string& propName, Type& refType);

        /**
         * \brief Create a new reference array property.
         * \param propName Property name.
         * \param refType Other type to reference.
         * \return New property.
         */
        Property& createReferenceArrayProperty(const std::string& propName, Type& refType);

        /**
         * \brief Create a new nested type property.
         * \param propName Property name.
         * \param nestedType Other type to use as member.
         * \return New property.
         */
        Property& createNestedTypeProperty(const std::string& propName, Type& nestedType);

        [[nodiscard]] std::vector<sql::Table*> getPrimitiveArrayTables() const;

        [[nodiscard]] std::vector<sql::Table*> getBlobArrayTables() const;

        [[nodiscard]] std::vector<sql::Table*> getReferenceArrayTables() const;

        ////////////////////////////////////////////////////////////////
        // Commit.
        ////////////////////////////////////////////////////////////////

        void requireNotCommitted() const;

        void requireCommitted() const;

        /**
         * \brief Commit this type to the library. Inserts entries into the type and property tables.
         */
        void commit();

        ////////////////////////////////////////////////////////////////
        // ...
        ////////////////////////////////////////////////////////////////

    private:
        ////////////////////////////////////////////////////////////////
        // Private methods.
        ////////////////////////////////////////////////////////////////

        void generate() const;

        Property&
          createProperty(const std::string& propName, DataType dataType, Type* refType, bool isArray, bool isBlob);

        void addProperty(PropertyPtr prop);

        /**
         * \brief Row ID.
         */
        sql::row_id id = -1;

        /**
         * \brief Namespace.
         */
        Namespace* nameSpace = nullptr;

        /**
         * \brief Type name.
         */
        std::string name;

        bool isInstance = true;

        /**
         * \brief Properties.
         */
        PropertyList properties;
    };
}  // namespace alex