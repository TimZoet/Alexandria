#pragma once

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "alexandria_testutils/utils.h"

class GetReferenceArray final : public utils::LibraryMember
{
public:
    void operator()() override;
};
