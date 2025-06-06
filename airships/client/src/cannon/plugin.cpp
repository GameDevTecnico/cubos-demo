#include "plugin.hpp"
#include "../player/plugin.hpp"
#include "../player_id/plugin.hpp"
#include "../interactable/plugin.hpp"
#include "../interpolation/plugin.hpp"
#include "../bullet/plugin.hpp"
#include "../holdable/plugin.hpp"

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/scene/scene.hpp>
#include <cubos/engine/input/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/audio/plugin.hpp>

#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/collisions/plugin.hpp>
#include <cubos/engine/collisions/shapes/box.hpp>
#include <cubos/engine/collisions/collision_layers.hpp>
#include <cubos/engine/collisions/collision_mask.hpp>
#include <cubos/engine/physics/plugin.hpp>
#include <cubos/engine/physics/plugins/gravity.hpp>

#include <iostream>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(airships::client::Cannon)
{
    return cubos::core::ecs::TypeBuilder<Cannon>("airships::client::Cannon")
        .withField("player", &Cannon::player)
        .withField("cannonLoaded", &Cannon::cannonLoaded)
        .withField("bulletScene", &Cannon::bulletScene)
        .withField("bulletSpeed", &Cannon::bulletSpeed)
        .build();
}

CUBOS_REFLECT_IMPL(airships::client::CannonTube)
{
    return cubos::core::ecs::TypeBuilder<CannonTube>("airships::client::CannonTube").build();
}

void airships::client::cannonPlugin(Cubos& cubos)
{
    cubos.depends(assetsPlugin);
    cubos.depends(inputPlugin);
    cubos.depends(transformPlugin);
    cubos.depends(physicsPlugin);
    cubos.depends(gravityPlugin);
    cubos.depends(playerPlugin);
    cubos.depends(playerIdPlugin);
    cubos.depends(interactablePlugin);
    cubos.depends(interpolationPlugin);
    cubos.depends(bulletPlugin);
    cubos.depends(collisionsPlugin);
    cubos.depends(audioPlugin);
    cubos.depends(holdablePlugin);

    cubos.component<Cannon>();
    cubos.component<CannonTube>();

    cubos.observer("add Interactable to Cannon").onAdd<Cannon>().call([](Commands cmds, Query<Entity> query) {
        for (auto [entity] : query)
        {
            cmds.add(entity, Interactable{});
        }
    });

    cubos.observer("handle Cannon interaction")
        .onAdd<Interaction>()
        .call([](Commands cmds, Query<Entity, Cannon&, Interaction&> query, Query<Player&, const PlayerId&> players,
                 Query<Entity, Holdable&, ChildOf&, InterpolationOf&> held) {
            for (auto [entity, cannon, interaction] : query)
            {
                cmds.remove<Interaction>(entity);
                auto [player, id] = *players.at(interaction.player);

                // If no player is controlling the cannon
                if (cannon.player == -1)
                {
                    // If player is holding a ball
                    auto match = held.pin(2, interaction.player).first();
                    if (match && !cannon.cannonLoaded)
                    {
                        auto [heldEnt, holdable, heldChildOf, interpolationOf] = *match;

                        // Item must be cannon ball to be loaded
                        if (holdable.type == "ball")
                        {
                            cannon.cannonLoaded = true;
                            cmds.destroy(heldEnt);
                            continue;
                        }
                    }
                    cannon.player = id.id;
                    player.interactingWith = entity;
                }
                else if (id.id == cannon.player)
                {
                    cannon.player = -1;
                    player.interactingWith = {};
                }
            }
        });

    cubos.system("cannon controls")
        .call(
            [](Input& inputs, const DeltaTime& dt, Query<CannonTube&, Rotation&, ChildOf&, Cannon&, Rotation&> query) {
                for (auto [tube, tubeRotation, childOf1, cannon, cannonRotation] : query)
                {
                    if (cannon.player == -1)
                        continue;

                    glm::quat yawQuat = glm::angleAxis(-inputs.axis("horizontal", cannon.player) * dt.value(),
                                                       glm::vec3(0.0F, 1.0F, 0.0F)); // Rotation around Y-axis
                    glm::quat pitchQuat = glm::angleAxis(inputs.axis("vertical", cannon.player) * dt.value(),
                                                         glm::vec3(0.0F, 0.0F, 1.0F)); // Rotation around X-axis

                    glm::vec3 cannonEuler = glm::eulerAngles(yawQuat * cannonRotation.quat);
                    cannonEuler.y = glm::clamp(cannonEuler.y, glm::radians(-45.0F), glm::radians(45.0F));
                    cannonRotation.quat = glm::quat(cannonEuler);

                    glm::vec3 tubeEuler = glm::eulerAngles(pitchQuat * tubeRotation.quat);
                    tubeEuler.z = glm::clamp(tubeEuler.z, glm::radians(-22.5F), 0.0F);
                    tubeRotation.quat = glm::quat(tubeEuler);

                    // cannon.cannonLoaded = true;
                }
            });

    cubos.system("fire cannon")
        .call([](Commands cmds, Assets& assets, Input& inputs,
                 Query<CannonTube&, Rotation&, ChildOf&, Entity, Cannon&, LocalToWorld&, ChildOf&, Velocity&> query,
                 Query<Entity, const InterpolationOf&> interpolationQuery) {
            for (auto [tube, tubeRotation, childOf1, cannonEnt, cannon, cannonLocalToWorld, childOf2, boatVelocity] :
                 query)
            {
                if (cannon.player == -1)
                {
                    continue;
                }

                if (inputs.justPressed("fire", cannon.player))
                {
                    if (!cannon.cannonLoaded)
                        continue;

                    if (auto match = interpolationQuery.pin(1, cannonEnt).first())
                    {
                        auto [ent, interpolationOf] = *match;
                        cmds.add(ent, AudioPlay{});
                    }

                    glm::vec3 cannonPosition = cannonLocalToWorld.worldPosition();
                    glm::quat cannonRotation = cannonLocalToWorld.worldRotation();

                    glm::vec3 forward = cannonRotation * tubeRotation.quat * glm::vec3(-1.0F, 0.0F, 0.0F);

                    auto ball =
                        cmds.create()
                            .add(Position{.vec = cannonPosition + forward * 10.0F})
                            .add(Rotation{.quat = cannonRotation})
                            .add(Scale{0.5F})
                            .add(PhysicsBundle{
                                .mass = 1.0F, .velocity = boatVelocity.vec, .impulse = forward * cannon.bulletSpeed})
                            .add(CollisionLayers{1})
                            .add(CollisionMask{1})
                            .add(BoxCollisionShape{.box = {.halfSize = {3.5F, 3.5F, 3.5F}}})
                            .add(Bullet{})
                            .add(Interpolated{cannon.bulletScene})
                            .entity();

                    cannon.cannonLoaded = false;
                }
            }
        });
}
