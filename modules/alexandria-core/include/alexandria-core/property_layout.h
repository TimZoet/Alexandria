#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <memory>
#include <string>

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "alexandria-core/data_type.h"
#include "alexandria-core/fwd.h"

namespace alex
{
    class Type;

    class PropertyLayout
    {
    public:
        friend class TypeLayout;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        PropertyLayout(
          TypeLayout& layout, std::string propName, DataType type, Type* refType, bool isArray, bool isBlob);

        PropertyLayout() = delete;

        PropertyLayout(const PropertyLayout&) = default;

        PropertyLayout(PropertyLayout&&) noexcept = default;

        ~PropertyLayout() noexcept = default;

        PropertyLayout& operator=(const PropertyLayout&) = default;

        PropertyLayout& operator=(PropertyLayout&&) noexcept = default;

        [[nodiscard]] bool operator==(const PropertyLayout& rhs) const noexcept;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Get property name.
         * \return Property name.
         */
        [[nodiscard]] const std::string& getName() const noexcept;

        /**
         * \brief Get data type.
         * \return DataType.
         */
        [[nodiscard]] DataType getDataType() const noexcept;

        /**
         * \brief Get type referenced by this property.
         * \return Pointer to type. Returns null if there is no type.
         */
        [[nodiscard]] Type* getReferenceType() noexcept;

        /**
         * \brief Get type.
         * \return Pointer to type. Returns null if there is no type.
         */
        [[nodiscard]] const Type* getReferenceType() const noexcept;

        /**
         * \brief Returns whether this property is an array.
         * \return True if array.
         */
        [[nodiscard]] bool isArray() const noexcept;

        /**
         * \brief Returns whether this property is a blob.
         * \return True if blob.
         */
        [[nodiscard]] bool isBlob() const noexcept;

    private:
        /**
         * \brief Commit this property to the library. Inserts entries into the property table.
         */
        [[nodiscard]] sql::row_id commit(Namespace& nameSpace, sql::row_id typeId) const;

        void generate(Library&                  library,
                      sql::row_id               currentType,
                      sql::Table&               instanceTable,
                      std::vector<sql::Table*>& primitiveArrayTables,
                      std::vector<sql::Table*>& blobArrayTables,
                      std::vector<sql::Table*>& referenceArrayTables,
                      const std::string&        prefix) const;

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Layout this property is part of.
         */
        TypeLayout* typeLayout = nullptr;

        /**
         * \brief Property name.
         */
        std::string name;

        /**
         * \brief Data type.
         */
        DataType dataType;

        /**
         * \brief Pointer to type. Only set if dataType == Reference or Nested.
         */
        Type* referenceType;

        /**
         * \brief Indicates property is an array type.
         */
        bool array;

        /**
         * \brief Indicates property is a blob type.
         */
        bool blob;
    };

    using PropertyLayoutPtr = std::unique_ptr<PropertyLayout>;
}  // namespace alex
