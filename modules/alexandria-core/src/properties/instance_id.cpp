#include "alexandria-core/properties/instance_id.h"

namespace alex
{
    void InstanceId::regenerate()
    {
        id = uuids::uuid_system_generator{}();
    }

}  // namespace alex
