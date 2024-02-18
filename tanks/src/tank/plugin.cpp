#include "plugin.hpp"
#include "tank.hpp"

#include <glm/glm.hpp>
#include <cubos/engine/transform/position.hpp>
#include <cubos/engine/transform/rotation.hpp>
#include <cubos/engine/input/input.hpp>

using namespace cubos::engine;

void tankPlugin(Cubos& cubos)
{
    cubos.addRelation<Tank>();
    cubos.addComponent<TankTower>();
    cubos.addComponent<TankBody>();


    cubos.system("move tanks")
        .call([](const DeltaTime& dt, const Input& input, Query<TankBody&, Position&, Rotation&, const Tank&, TankTower&, Position&, Rotation&> query)  {
            for (auto [body, bodyPosition, bodyRotation, tank, tower, towerPosition, towerRotation] : query) {
                auto throttle = glm::clamp(input.axis("throttle", tank.player), 0.0F, 1.0F);
                auto reverse = glm::clamp(input.axis("reverse", tank.player), 0.0F, 1.0F);
                auto steer = glm::clamp(input.axis("steer", tank.player), 0.0F, 1.0F);
                auto towerSteer = input.axis("tower-rotaytion", tank.player);

                auto movement = throttle + reverse;

                auto forward = bodyRotation.quat * glm::vec3(0.0F, 0.0F, 1.0F);

                auto towerRotationDelta = glm::angleAxis(towerSteer * tower.rotationVelocity * dt.value, glm::vec3(0.0F, 1.0F, 0.0F));
                towerRotation.quat = towerRotationDelta * towerRotation.quat;

                auto rotationDelta = glm::angleAxis(steer * body.angularVelocity * dt.value, glm::vec3(0.0F, 1.0F, 0.0F));
                bodyRotation.quat = rotationDelta * bodyRotation.quat;
                bodyPosition.vec += forward * body.speed * movement * dt.value;
                towerPosition.vec[0] = bodyPosition.vec[0];
                towerPosition.vec[2] = bodyPosition.vec[2];
            }
        });
}
