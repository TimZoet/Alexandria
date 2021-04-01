#pragma once

#include "alexandria/library.h"

struct Foo
{
    int32_t x = 0, y = 0;
};

struct Type0
{
    int64_t                      id = 0;
    float                        x  = 0;
    int32_t                      y  = 0;
    alex::Blob<std::vector<Foo>> foo;
};

struct Type1
{
    int64_t                     id = 0;
    alex::ReferenceArray<Type0> refs;
};