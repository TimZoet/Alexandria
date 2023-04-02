#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <string>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "cppql/core/column.h"

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
        Blob,
        Reference,
        Nested
    };

    /*
     * float
     * float[]
     * float*
     * int32
     * int32[]
     * int32*
     * string
     * string[]
     * blob
     * blob[]
     * nested
     * reference
     * reference[]
     */

    [[nodiscard]] bool isPrimitiveDataType(DataType dataType) noexcept;

    [[nodiscard]] sql::Column::Type toColumnType(DataType dataType) noexcept;

    [[nodiscard]] std::string toString(DataType dataType);

    void fromString(const std::string& s, DataType& dataType);
}  // namespace alex