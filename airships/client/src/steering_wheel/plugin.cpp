#include "plugin.hpp"
#include "../drivable/plugin.hpp"

#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/ecs/reflection.hpp>

#include <cubos/engine/input/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(airships::client::SteeringWheel)
{
    return cubos::core::ecs::TypeBuilder<SteeringWheel>("airships::client::SteeringWheel")
        .withField("player", &SteeringWheel::player)
        .withField("turnAxis", &SteeringWheel::turnAxis)
        .withField("maxTurnAngle", &SteeringWheel::maxTurnAngle)
        .withField("turnAngle", &SteeringWheel::turnAngle)
        .withField("turnSpeed", &SteeringWheel::turnSpeed)
        .withField("targetLerpFactor", &SteeringWheel::targetLerpFactor)
        .build();
}

CUBOS_REFLECT_IMPL(airships::client::SteeringWheelHead)
{
    return cubos::core::ecs::TypeBuilder<SteeringWheelHead>("airships::client::SteeringWheelHead").build();
}

void airships::client::steeringWheelPlugin(Cubos& cubos)
{
    cubos.depends(inputPlugin);
    cubos.depends(transformPlugin);
    cubos.depends(drivablePlugin);

    cubos.component<SteeringWheel>();
    cubos.component<SteeringWheelHead>();

    cubos.system("update SteeringWheel entities")
        .call([](const DeltaTime& dt, Input& input,
                 Query<const SteeringWheelHead&, Rotation&, const ChildOf&, SteeringWheel&, const ChildOf&, Drivable&>
                     query) {
            for (auto [head, rotation, childOf1, wheel, childOf2, drivable] : query)
            {
                float turnInput = wheel.player == -1 ? 0.0F : input.axis(wheel.turnAxis.c_str(), wheel.player);

                // Update target turn angle
                wheel.targetTurnAngle += turnInput * wheel.turnSpeed * dt.value();
                wheel.targetTurnAngle = glm::clamp(wheel.targetTurnAngle, -wheel.maxTurnAngle, wheel.maxTurnAngle);

                // Smoothly turn the actual wheel towards the target turn angle
                wheel.turnAngle = glm::mix(wheel.turnAngle, wheel.targetTurnAngle, 1 - glm::pow(1.0 - wheel.targetLerpFactor, dt.value()));

                // Update the drivable entity
                drivable.targetAngularVelocity = (wheel.turnAngle / wheel.maxTurnAngle) * drivable.topAngularVelocity;
                 

                // Animate the wheel
                rotation.quat = glm::angleAxis(glm::radians(wheel.turnAngle), glm::vec3{0.0F, 0.0F, 1.0F});
            }
        });
}