#pragma once

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "alexandria_testutils/utils.h"

class ReferenceSearch final : public utils::LibraryMember
{
public:
    void operator()() override;
};
