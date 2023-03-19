#include "alexandria-core/member_types/instance_id.h"

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include "uuid_system_generator.h"

namespace alex
{
    void InstanceId::regenerate() { id = uuids::uuid_system_generator{}(); }

}  // namespace alex
