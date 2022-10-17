#include "alexandria/data_type.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

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
        case DataType::Blob:
        case DataType::String:
        case DataType::Type: return false;
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
        case DataType::Type: return sql::Column::Type::Int;
        case DataType::Blob: return sql::Column::Type::Blob;
        }

        return sql::Column::Type::Null;
    }

    std::string toString(const DataType dataType)
    {
        switch (dataType)
        {
        case DataType::Int32: return "Int32";
        case DataType::Uint32: return "Uint32";
        case DataType::Int64: return "Int64";
        case DataType::Uint64: return "Uint64";
        case DataType::Float: return "Float";
        case DataType::Double: return "Double";
        case DataType::String: return "String";
        case DataType::Type: return "Type";
        case DataType::Blob: return "Blob";
        }

        return "";
    }

    void fromString(const std::string& s, DataType& dataType)
    {
        if (s == "Int32")
            dataType = DataType::Int32;
        else if (s == "Uint32")
            dataType = DataType::Uint32;
        else if (s == "Int64")
            dataType = DataType::Int64;
        else if (s == "Uint64")
            dataType = DataType::Uint64;
        else if (s == "Float")
            dataType = DataType::Float;
        else if (s == "Double")
            dataType = DataType::Double;
        else if (s == "String")
            dataType = DataType::String;
        else if (s == "Type")
            dataType = DataType::Type;
        else if (s == "Blob")
            dataType = DataType::Blob;
        else
            throw std::runtime_error("");
    }
}  // namespace alex