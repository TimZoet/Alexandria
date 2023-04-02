#include "alexandria-core/data_type.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <cassert>
#include <stdexcept>

namespace alex
{
    bool isPrimitiveDataType(const DataType dataType) noexcept
    {
        switch (dataType)
        {
        case DataType::Int32:
        case DataType::Uint32:
        case DataType::Int64:
        case DataType::Uint64:
        case DataType::Float:
        case DataType::Double: return true;
        case DataType::String:
        case DataType::Blob:
        case DataType::Nested:
        case DataType::Reference: return false;
        }

        return false;
    }

    sql::Column::Type toColumnType(const DataType dataType) noexcept
    {
        switch (dataType)
        {
        case DataType::Int32:
        case DataType::Uint32:
        case DataType::Int64:
        case DataType::Uint64: return sql::Column::Type::Int;
        case DataType::Float:
        case DataType::Double: return sql::Column::Type::Real;
        case DataType::String: return sql::Column::Type::Text;
        case DataType::Blob: return sql::Column::Type::Blob;
        case DataType::Nested: return sql::Column::Type::Null;
        case DataType::Reference: return sql::Column::Type::Int;
        }
        assert(false);
        return sql::Column::Type::Null;
    }

    std::string toString(const DataType dataType)
    {
        switch (dataType)
        {
        case DataType::Int32: return "int32";
        case DataType::Uint32: return "uint32";
        case DataType::Int64: return "int64";
        case DataType::Uint64: return "uint64";
        case DataType::Float: return "float";
        case DataType::Double: return "double";
        case DataType::String: return "string";
        case DataType::Blob: return "blob";
        case DataType::Nested: return "nested";
        case DataType::Reference: return "reference";
        }

        throw std::runtime_error("");
    }

    void fromString(const std::string& s, DataType& dataType)
    {
        if (s == "int32")
            dataType = DataType::Int32;
        else if (s == "uint32")
            dataType = DataType::Uint32;
        else if (s == "int64")
            dataType = DataType::Int64;
        else if (s == "uint64")
            dataType = DataType::Uint64;
        else if (s == "float")
            dataType = DataType::Float;
        else if (s == "double")
            dataType = DataType::Double;
        else if (s == "string")
            dataType = DataType::String;
        else if (s == "blob")
            dataType = DataType::Blob;
        else if (s == "nested")
            dataType = DataType::Nested;
        else if (s == "reference")
            dataType = DataType::Reference;
        else
            throw std::runtime_error("");
    }
}  // namespace alex