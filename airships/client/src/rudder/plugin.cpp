#include "plugin.hpp"
#include "../drivable/plugin.hpp"

#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/ecs/reflection.hpp>

#include <cubos/engine/transform/plugin.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(airships::client::Rudder)
{
    return cubos::core::ecs::TypeBuilder<Rudder>("airships::client::Rudder")
        .withField("maxYaw", &Rudder::maxYaw)
        .withField("maxPitch", &Rudder::maxPitch)
        .withField("offsetYaw", &Rudder::offsetYaw)
        .withField("offsetPitch", &Rudder::offsetPitch)
        .build();
}

void airships::client::rudderPlugin(Cubos& cubos)
{
    cubos.depends(transformPlugin);
    cubos.depends(drivablePlugin);

    cubos.component<Rudder>();

    cubos.system("animate Rudder entities").call([](Query<const Rudder&, Rotation&, const ChildOf&, Drivable&> query) {
        for (auto [rudder, rotation, childOf, drivable] : query)
        {
            float yaw = (drivable.targetAngularVelocity / drivable.topAngularVelocity) * rudder.maxYaw;
            float pitch = (drivable.targetAngularVelocity / drivable.topAngularVelocity) * rudder.maxPitch;
            rotation.quat = glm::angleAxis(glm::radians(yaw + rudder.offsetYaw), glm::vec3{0.0F, 1.0F, 0.0F});
            rotation.quat *= glm::angleAxis(glm::radians(pitch + rudder.offsetPitch), glm::vec3{1.0F, 0.0F, 0.0F});
        }
    }); 
}
