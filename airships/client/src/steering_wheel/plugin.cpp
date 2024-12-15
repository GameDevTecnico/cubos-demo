#include "plugin.hpp"
#include "../player/plugin.hpp"
#include "../player_id/plugin.hpp"
#include "../drivable/plugin.hpp"
#include "../interactable/plugin.hpp"
#include "../interpolation/plugin.hpp"

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
        .withField("startAutoTurnDistance", &SteeringWheel::startAutoTurnDistance)
        .withField("endAutoTurnDistance", &SteeringWheel::endAutoTurnDistance)
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
    cubos.depends(playerPlugin);
    cubos.depends(playerIdPlugin);
    cubos.depends(interactablePlugin);
    cubos.depends(interpolationPlugin);

    cubos.component<SteeringWheel>();
    cubos.component<SteeringWheelHead>();

    cubos.observer("add Interactable to SteeringWheel")
        .onAdd<SteeringWheel>()
        .call([](Commands cmds, Query<Entity> query) {
            for (auto [entity] : query)
            {
                cmds.add(entity, Interactable{});
            }
        });

    cubos.observer("handle SteeringWheel interaction")
        .onAdd<Interaction>()
        .call([](Commands cmds, Query<Entity, SteeringWheel&, Interaction&> query,
                 Query<Player&, const PlayerId&> players) {
            for (auto [entity, wheel, interaction] : query)
            {
                cmds.remove<Interaction>(entity);
                auto [player, id] = *players.at(interaction.player);

                if (wheel.player == -1)
                {
                    wheel.player = id.id;
                    player.interactingWith = entity;
                }
                else if (id.id == wheel.player)
                {
                    wheel.player = -1;
                    player.interactingWith = {};
                }
            }
        });

    cubos.system("update SteeringWheel entities")
        .call([](const DeltaTime& dt, Input& input,
                 Query<const LocalToWorld&, const SteeringWheelHead&, Rotation&, const ChildOf&, const InterpolationOf&,
                       SteeringWheel&, const ChildOf&, Drivable&>
                     query) {
            for (auto [ltw, head, rotation, childOf1, interpolationOf, wheel, childOf2, drivable] : query)
            {
                float turnInput = wheel.player == -1 ? 0.0F : input.axis(wheel.turnAxis.c_str(), wheel.player);

                // Update target turn angle
                wheel.targetTurnAngle += turnInput * wheel.turnSpeed * dt.value();
                wheel.targetTurnAngle = glm::clamp(wheel.targetTurnAngle, -wheel.maxTurnAngle, wheel.maxTurnAngle);

                // Turn wheel automatically if outside of the auto turn distance
                auto currentPosition = ltw.worldPosition();
                auto distanceToCenter =
                    glm::distance(glm::vec3{currentPosition.x, 0.0F, currentPosition.z}, glm::vec3{0.0F, 0.0F, 0.0F});
                if (distanceToCenter > wheel.startAutoTurnDistance)
                {
                    auto ratio = (distanceToCenter - wheel.startAutoTurnDistance) /
                                 (wheel.endAutoTurnDistance - wheel.startAutoTurnDistance);
                    ratio = glm::clamp(ratio, 0.0F, 1.0F);

                    // Calculate the angle between the current drivable direction and the direction to the center
                    auto forward = glm::vec2(glm::sin(glm::radians(drivable.yaw)), glm::cos(glm::radians(drivable.yaw)));
                    auto toCenter = glm::normalize(glm::vec2{-currentPosition.x, -currentPosition.z});
                    auto diff = glm::degrees(glm::atan(toCenter.y, toCenter.x) - glm::atan(forward.y, forward.x)) / 180.0F;

                    // Turn the wheel so that the drivable starts turning back
                    wheel.targetTurnAngle = glm::mix(wheel.targetTurnAngle, diff * wheel.maxTurnAngle, ratio);
                }

                // Smoothly turn the actual wheel towards the target turn angle
                wheel.turnAngle = glm::mix(wheel.turnAngle, wheel.targetTurnAngle,
                                           1 - glm::pow(1.0 - wheel.targetLerpFactor, dt.value()));

                // Update the drivable entity
                drivable.targetAngularVelocity = (wheel.turnAngle / wheel.maxTurnAngle) * drivable.topAngularVelocity;

                // Animate the wheel
                rotation.quat = glm::angleAxis(glm::radians(wheel.turnAngle), glm::vec3{0.0F, 0.0F, 1.0F});
            }
        });
}