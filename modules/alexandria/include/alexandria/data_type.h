#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <string>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "cppql-core/column.h"

namespace alex
{
    enum class DataType
    {
        Int32,
        Uint32,
        Int64,
        Uint64,
        Float,
        Double,
        String,
        Type,
        Blob
    };

    bool isPrimitiveDataType(DataType dataType) noexcept;

    sql::Column::Type toColumnType(DataType dataType) noexcept;

    std::string toString(DataType dataType);

    void fromString(const std::string& s, DataType& dataType);
}  // namespace alex