#pragma once

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "alexandria_test/utils.h"

class DeleteReferenceArray final : public utils::LibraryMember
{
public:
    void operator()() override;
};
