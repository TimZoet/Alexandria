#pragma once

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "alexandria_test/utils.h"

class DeleteInvalid final : public utils::LibraryMember
{
public:
    void operator()() override;
};
