#include "plugin.hpp"
#include "../drivable/plugin.hpp"

#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/ecs/reflection.hpp>

#include <cubos/engine/transform/plugin.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(airships::client::Rudder)
{
    return cubos::core::ecs::TypeBuilder<Rudder>("airships::client::Rudder")
        .withField("maxAngle", &Rudder::maxAngle)
        .withField("offsetAngle", &Rudder::offsetAngle)
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
            float angle = (drivable.targetAngularVelocity / drivable.topAngularVelocity) * rudder.maxAngle;
            rotation.quat = glm::angleAxis(glm::radians(angle + rudder.offsetAngle), glm::vec3{0.0F, 1.0F, 0.0F});
        }
    }); 
}
