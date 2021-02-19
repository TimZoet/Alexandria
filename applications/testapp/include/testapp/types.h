#pragma once

#include "alexandria/library.h"

struct Type0
{
    int64_t                      id = 0;
    alex::PrimitiveBlob<int32_t> array;
};

struct Type1
{
    int64_t                id = 0;
    alex::Reference<Type0> type0;
};