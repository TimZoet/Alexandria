#pragma once

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "alexandria_test/utils.h"

class UpdatePrimitive final : public utils::LibraryMember
{
public:
    void operator()() override;
};