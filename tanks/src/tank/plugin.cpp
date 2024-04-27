#include "plugin.hpp"
#include "tank.hpp"

#include <glm/glm.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/input/plugin.hpp>

using namespace cubos::engine;

void tankPlugin(Cubos& cubos)
{
    cubos.depends(transformPlugin);
    cubos.depends(inputPlugin);

    cubos.component<Tank>();
    cubos.component<Turret>();

    cubos.system("move tanks")
        .call([](const DeltaTime& dt, const Input& input, Query<Tank&, Position&, Rotation&> query) {
            for (auto [tank, position, rotation] : query)
            {
                auto throttle = glm::clamp(input.axis("throttle", tank.player), 0.0F, 1.0F);
                auto reverse = glm::clamp(input.axis("reverse", tank.player), 0.0F, 1.0F);
                auto steer = -glm::clamp(input.axis("steer", tank.player), -1.0F, 1.0F);

                auto movement = throttle - reverse;
                auto forward = rotation.quat * glm::vec3(1.0F, 0.0F, 0.0F);

                auto rotationDelta =
                    glm::angleAxis(steer * tank.angularVelocity * dt.value(), glm::vec3(0.0F, 1.0F, 0.0F));
                rotation.quat = rotationDelta * rotation.quat;
                position.vec += forward * tank.speed * movement * dt.value();
            }
        });

    cubos.system("rotate turrets").call([](const DeltaTime& dt, const Input& input, Query<Turret&, Rotation&> query) {
        for (auto [tower, rotation] : query)
        {
            auto steer = input.axis("tower-rotation", tower.player);

            auto towerRotationDelta =
                glm::angleAxis(steer * tower.rotationVelocity * dt.value(), glm::vec3(0.0F, 1.0F, 0.0F));
            rotation.quat = towerRotationDelta * rotation.quat;
        }
    });
}
