#include "plugin.hpp"
#include "../drivable/plugin.hpp"
#include "../interactable/plugin.hpp"
#include "../interpolation/plugin.hpp"
#include "../player/plugin.hpp"
#include "../player_id/plugin.hpp"

#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/ecs/reflection.hpp>

#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/physics/plugin.hpp>
#include <cubos/engine/physics/plugins/gravity.hpp>
#include <cubos/engine/fixed_step/plugin.hpp>
#include <cubos/engine/input/plugin.hpp>

#include <glm/common.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(airships::client::Boiler)
{
    return cubos::core::ecs::TypeBuilder<Boiler>("airships::client::Boiler")
        .withField("player", &Boiler::player)
        .withField("leverAxis", &Boiler::leverAxis)
        .withField("leverSpeed", &Boiler::leverSpeed)
        .withField("leverPosition", &Boiler::leverPosition)
        .withField("targetLeverPosition", &Boiler::targetLeverPosition)
        .withField("leverLerpFactor", &Boiler::leverLerpFactor)
        .withField("maxBuoyancy", &Boiler::maxBuoyancy)
        .withField("minBuoyancy", &Boiler::minBuoyancy)
        .build();
}

CUBOS_REFLECT_IMPL(airships::client::BoilerLeverCollider)
{
    return cubos::core::ecs::TypeBuilder<BoilerLeverCollider>("airships::client::BoilerLeverCollider").build();
}

CUBOS_REFLECT_IMPL(airships::client::BoilerLeverAnimator)
{
    return cubos::core::ecs::TypeBuilder<BoilerLeverAnimator>("airships::client::BoilerLeverAnimator")
        .withField("minHeight", &BoilerLeverAnimator::minHeight)
        .withField("maxHeight", &BoilerLeverAnimator::maxHeight)
        .build();
}

void airships::client::boilerPlugin(cubos::engine::Cubos& cubos)
{
    cubos.depends(transformPlugin);
    cubos.depends(drivablePlugin);
    cubos.depends(interactablePlugin);
    cubos.depends(physicsPlugin);
    cubos.depends(inputPlugin);
    cubos.depends(fixedStepPlugin);
    cubos.depends(gravityPlugin);
    cubos.depends(playerPlugin);
    cubos.depends(playerIdPlugin);
    cubos.depends(interpolationPlugin);

    cubos.component<Boiler>();
    cubos.component<BoilerLeverCollider>();
    cubos.component<BoilerLeverAnimator>();

    cubos.observer("add Interactable to BoilerLevelCollider")
        .onAdd<BoilerLeverCollider>()
        .call([](Commands cmds, Query<Entity> query) {
            for (auto [entity] : query)
            {
                cmds.add(entity, Interactable{});
            }
        });

    cubos.observer("initialize lever position on Boiler").onAdd<Boiler>().call([](Commands cmds, Query<Boiler&> query) {
        for (auto [boiler] : query)
        {
            // Figure out the lever position which sets the buoyancy to 1
            float f = boiler.maxBuoyancy - boiler.minBuoyancy;
            float r = (1.0 - boiler.minBuoyancy) / f;
            boiler.leverPosition = 2.0 * r - 1.0;
            boiler.targetLeverPosition = boiler.leverPosition;
        }
    });

    cubos.observer("handle BoilerLevelCollider interaction")
        .onAdd<Interaction>()
        .call([](Commands cmds, Query<Entity, BoilerLeverCollider&, Interaction&, const ChildOf&, Boiler&> query,
                 Query<Player&, PlayerId&> players) {
            for (auto [entity, boilerCollider, interaction, childOf, boiler] : query)
            {
                cmds.remove<Interaction>(entity);
                auto [player, id] = *players.at(interaction.player);

                if (boiler.player == -1)
                {
                    boiler.player = id.id;
                    player.interactingWith = entity;
                }
                else if (id.id == boiler.player)
                {
                    boiler.player = -1;
                    player.interactingWith = {};
                }
            }
        });

    cubos.system("update Boiler entities")
        .call([](const DeltaTime& dt, Input& input,
                 Query<const BoilerLeverAnimator&, Position&, const ChildOf&, const InterpolationOf&, Boiler&,
                       const ChildOf&, Drivable&>
                     query) {
            for (auto [animator, position, childOf1, interpolationOf, boiler, childOf2, drivable] : query)
            {
                float leverInput = boiler.player == -1 ? 0.0F : input.axis(boiler.leverAxis.c_str(), boiler.player);

                // Update target lever position
                boiler.targetLeverPosition -= leverInput * boiler.leverSpeed * dt.value();
                boiler.targetLeverPosition = glm::clamp(boiler.targetLeverPosition, -1.0F, 1.0F);

                // Smoothly interpolate the lever position into the target lever position
                boiler.leverPosition = glm::mix(boiler.leverPosition, boiler.targetLeverPosition,
                                                1 - glm::pow(1.0 - boiler.leverLerpFactor, dt.value()));

                // Update the drivable entity
                drivable.buoyancy = glm::mix(boiler.minBuoyancy, boiler.maxBuoyancy, 0.5 + 0.5 * boiler.leverPosition);

                // Animate the lever
                position.vec.y = glm::mix(animator.minHeight, animator.maxHeight, 0.5 + 0.5 * boiler.leverPosition);
            }
        });
}