#include "alexandria-core/type.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "cppql/include_all.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include "alexandria-core/library.h"

namespace alex
{
    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    sql::row_id Type::getId() const noexcept { return id; }

    const TypeLayout& Type::getLayout() const noexcept { return *typeLayout; }

    const std::string& Type::getName() const noexcept { return name; }

    TypeLayout::Instantiable Type::isInstantiable() const noexcept { return instantiable; }

    Namespace& Type::getNamespace() noexcept { return *nameSpace; }

    const Namespace& Type::getNamespace() const noexcept { return *nameSpace; }

    sql::Table& Type::getInstanceTable() const { return *tables.instance; }

    const std::vector<sql::Table*>& Type::getPrimitiveArrayTables() const { return tables.primitiveArrays; }

    const std::vector<sql::Table*>& Type::getBlobArrayTables() const { return tables.blobArrays; }

    const std::vector<sql::Table*>& Type::getReferenceArrayTables() const { return tables.referenceArrays; }
}  // namespace alex
