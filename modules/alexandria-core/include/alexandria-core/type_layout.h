#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <string>
#include <vector>

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "alexandria-core/fwd.h"
#include "alexandria-core/property_layout.h"

namespace alex
{
    class Type;

    class TypeLayout
    {
    public:
        friend class Library;

        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        enum class Instantiable
        {
            // Type is not instantiable and no tables are generated.
            False = 0,
            // Type is instantiable.
            True = 1
        };

        enum class Commit
        {
            // Type was created.
            Created = 1,
            // Type already existed.
            Existed = 2
        };

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        TypeLayout() = default;

        TypeLayout(const TypeLayout& other);

        TypeLayout(TypeLayout&&) noexcept = default;

        ~TypeLayout() = default;

        TypeLayout& operator=(const TypeLayout& other);

        TypeLayout& operator=(TypeLayout&&) = default;

        [[nodiscard]] bool operator==(const TypeLayout& rhs) const noexcept;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] const std::vector<PropertyLayoutPtr>& getProperties() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Properties.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Create a new property with primitive data type.
         * \param propName Property name.
         * \param dataType Data type.
         * \return New property.
         */
        PropertyLayout& createPrimitiveProperty(const std::string& propName, DataType dataType);

        /**
         * \brief Create a new array property with primitive data type.
         * \param propName Property name.
         * \param dataType Data type.
         * \return New property.
         */
        PropertyLayout& createPrimitiveArrayProperty(const std::string& propName, DataType dataType);

        /**
         * \brief Create a new primitive blob array property.
         * \param propName Property name.
         * \param dataType Data type.
         * \return New property.
         */
        PropertyLayout& createPrimitiveBlobProperty(const std::string& propName, DataType dataType);

        /**
         * \brief Create a new property with string data type.
         * \param propName Property name.
         * \return New property.
         */
        PropertyLayout& createStringProperty(const std::string& propName);

        /**
         * \brief Create a new array property with string data type.
         * \param propName Property name.
         * \return New property.
         */
        PropertyLayout& createStringArrayProperty(const std::string& propName);

        /**
         * \brief Create a new property with blob data type.
         * \param propName Property name.
         * \return New property.
         */
        PropertyLayout& createBlobProperty(const std::string& propName);

        /**
         * \brief Create a new array property with blob data type.
         * \param propName Property name.
         * \return New property.
         */
        PropertyLayout& createBlobArrayProperty(const std::string& propName);

        /**
         * \brief Create a new reference property.
         * \param propName Property name.
         * \param refType Other type to reference.
         * \return New property.
         */
        PropertyLayout& createReferenceProperty(const std::string& propName, Type& refType);

        /**
         * \brief Create a new reference array property.
         * \param propName Property name.
         * \param refType Other type to reference.
         * \return New property.
         */
        PropertyLayout& createReferenceArrayProperty(const std::string& propName, Type& refType);

        /**
         * \brief Create a new nested type property.
         * \param propName Property name.
         * \param nestedType Other type to use as member.
         * \return New property.
         */
        PropertyLayout& createNestedTypeProperty(const std::string& propName, Type& nestedType);

        ////////////////////////////////////////////////////////////////
        // Commit.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Commit layout to a namespace with the given name. If an identical type already exists, no new type
         * is created. If a type with the same name but different layout already exists, an exception is thrown.
         * \param nameSpace Namespace to commit layout to.
         * \param name Unique type name.
         * \param instantiable Whether instances of this type can be created, or if it is only to be used as a nested
         * type. If false, no tables are generated for this type.
         * \return Pair of enum indicating whether type was created or already existed and pointer to (new) type.
         */
        std::pair<Commit, Type*>
          commit(Namespace& nameSpace, std::string name, Instantiable instantiable = Instantiable::True) const;

    private:
        ////////////////////////////////////////////////////////////////
        // Private methods.
        ////////////////////////////////////////////////////////////////

        PropertyLayout&
          createProperty(const std::string& propName, DataType dataType, Type* refType, bool isArray, bool isBlob);

        void addProperty(PropertyLayoutPtr prop);

        /**
         * \brief Properties.
         */
        std::vector<PropertyLayoutPtr> properties;
    };
}  // namespace alex