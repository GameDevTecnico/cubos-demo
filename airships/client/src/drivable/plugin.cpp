#include "plugin.hpp"

#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/ecs/reflection.hpp>

#include <cubos/engine/input/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(airships::client::Drivable)
{
    return cubos::core::ecs::TypeBuilder<Drivable>("airships::client::Drivable")
        .withField("topLinearVelocity", &Drivable::topLinearVelocity)
        .withField("linearVelocity", &Drivable::linearVelocity)
        .withField("linearAcceleration", &Drivable::linearAcceleration)
        .withField("topAngularVelocity", &Drivable::topAngularVelocity)
        .withField("angularVelocity", &Drivable::angularVelocity)
        .withField("targetAngularVelocity", &Drivable::targetAngularVelocity)
        .withField("angularAcceleration", &Drivable::angularAcceleration)
        .withField("maxRoll", &Drivable::maxRoll)
        .withField("roll", &Drivable::roll)
        .withField("rollLerpFactor", &Drivable::rollLerpFactor)
        .withField("yaw", &Drivable::yaw)
        .build();
}

void airships::client::drivablePlugin(Cubos& cubos)
{
    cubos.depends(inputPlugin);
    cubos.depends(transformPlugin);

    cubos.component<Drivable>();

    cubos.system("move Drivable entities")
        .before(transformUpdateTag)
        .call([](Input& input, Query<Drivable&, Position&, Rotation&> drivables, const DeltaTime& dt) {
            for (auto [drivable, position, rotation] : drivables)
            {
                // Update angular velocity
                auto missingAngularVelocity = drivable.targetAngularVelocity - drivable.angularVelocity;
                auto availableAngularVelocity = drivable.angularAcceleration * dt.value();
                drivable.angularVelocity +=
                    glm::clamp(missingAngularVelocity, -availableAngularVelocity, availableAngularVelocity);

                auto topAngularVelocity =
                    (glm::abs(drivable.linearVelocity) / drivable.topLinearVelocity) * drivable.topAngularVelocity;
                drivable.angularVelocity =
                    glm::clamp(drivable.angularVelocity, -topAngularVelocity, topAngularVelocity);

                // Apply angular velocity and compute rotation
                drivable.yaw -= drivable.angularVelocity * dt.value();
                drivable.yaw = glm::mod(drivable.yaw, 360.0F);
                rotation.quat = glm::angleAxis(glm::radians(drivable.yaw), glm::vec3{0.0F, 1.0F, 0.0F});

                auto rollFactor = drivable.angularVelocity / drivable.topAngularVelocity;
                auto targetRoll = rollFactor * drivable.maxRoll;
                drivable.roll = glm::mix(drivable.roll, targetRoll, 1 - glm::pow(1.0 - drivable.rollLerpFactor, dt.value()));
                rotation.quat *= glm::angleAxis(glm::radians(drivable.roll), glm::vec3{0.0F, 0.0F, 1.0F});

                // Update linear velocity
                drivable.linearVelocity += drivable.linearAcceleration * dt.value();
                drivable.linearVelocity =
                    glm::clamp(drivable.linearVelocity, -drivable.topLinearVelocity, drivable.topLinearVelocity);

                // Update position
                auto forward = rotation.quat * glm::vec3{0.0F, 0.0F, 1.0F};
                position.vec += forward * drivable.linearVelocity * dt.value();
            }
        });
}