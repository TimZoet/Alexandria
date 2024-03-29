#include "alexandria-core/properties/instance_id.h"

#include "uuid_system_generator.h"

namespace alex
{
    void InstanceId::regenerate()
    {
        id = uuids::uuid_system_generator{}();
    }

}  // namespace alex
