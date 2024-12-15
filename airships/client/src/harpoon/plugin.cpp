#include "plugin.hpp"
#include "../player/plugin.hpp"
#include "../player_id/plugin.hpp"
#include "../interactable/plugin.hpp"
#include "../interpolation/plugin.hpp"
#include "../bullet/plugin.hpp"

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/scene/scene.hpp>
#include <cubos/engine/input/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>

#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/collisions/plugin.hpp>
#include <cubos/engine/collisions/collider.hpp>
#include <cubos/engine/collisions/shapes/box.hpp>
#include <cubos/engine/physics/plugin.hpp>
#include <cubos/engine/physics/plugins/gravity.hpp>

#include <iostream>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(airships::client::Harpoon)
{
    return cubos::core::ecs::TypeBuilder<Harpoon>("airships::client::Harpoon")
        .withField("player", &Harpoon::player)
        .withField("harpoonLoaded", &Harpoon::harpoonLoaded)
        .withField("bulletScene", &Harpoon::bulletScene)
        .withField("bulletSpeed", &Harpoon::bulletSpeed)
        .build();
}

CUBOS_REFLECT_IMPL(airships::client::HarpoonTube)
{
    return cubos::core::ecs::TypeBuilder<HarpoonTube>("airships::client::HarpoonTube").build();
}

CUBOS_REFLECT_IMPL(airships::client::Arrow)
{
    return cubos::core::ecs::TypeBuilder<Arrow>("airships::client::Arrow").build();
}

void airships::client::harpoonPlugin(Cubos& cubos)
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

    cubos.component<Harpoon>();
    cubos.component<HarpoonTube>();
    cubos.component<Arrow>();

    cubos.observer("add Interactable to Harpoon").onAdd<Harpoon>().call([](Commands cmds, Query<Entity> query) {
        for (auto [entity] : query)
        {
            cmds.add(entity, Interactable{});
        }
    });

    cubos.observer("handle Harpoon interaction")
        .onAdd<Interaction>()
        .call([](Commands cmds, Query<Entity, Harpoon&, Interaction&> query, Query<Player&, const PlayerId&> players) {
            for (auto [entity, harpoon, interaction] : query)
            {
                cmds.remove<Interaction>(entity);
                auto [player, id] = *players.at(interaction.player);

                if (harpoon.player == -1)
                {
                    harpoon.player = id.id;
                    player.interactingWith = entity;
                }
                else if (id.id == harpoon.player)
                {
                    harpoon.player = -1;
                    player.interactingWith = {};
                }
            }
        });

    cubos.system("harpoon controls")
        .call(
            [](Input& inputs, const DeltaTime& dt, Query<HarpoonTube&, Rotation&, ChildOf&, Harpoon&, Rotation&> query) {
                for (auto [tube, tubeRotation, childOf1, harpoon, harpoonRotation] : query)
                {
                    if (harpoon.player == -1)
                        continue;

                    glm::quat yawQuat = glm::angleAxis(-inputs.axis("horizontal", harpoon.player) * dt.value(),
                                                       glm::vec3(0.0F, 1.0F, 0.0F)); // Rotation around Y-axis
                    glm::quat pitchQuat = glm::angleAxis(inputs.axis("vertical", harpoon.player) * dt.value(),
                                                         glm::vec3(0.0F, 0.0F, 1.0F)); // Rotation around X-axis

                    glm::vec3 harpoonEuler = glm::eulerAngles(yawQuat * harpoonRotation.quat);
                    harpoonEuler.y = glm::clamp(harpoonEuler.y, glm::radians(45.0F), glm::radians(135.0F));
                    harpoonRotation.quat = glm::quat(harpoonEuler);

                    glm::vec3 tubeEuler = glm::eulerAngles(pitchQuat * tubeRotation.quat);
                    tubeEuler.z = glm::clamp(tubeEuler.z, glm::radians(-22.5F), 0.0F);
                    tubeRotation.quat = glm::quat(tubeEuler);

                    harpoon.harpoonLoaded = true;
                }
            });

    cubos.system("fire harpoon")
        .call([](Commands cmds, Assets& assets, Input& inputs,
                 Query<Entity, Arrow&, ChildOf&, HarpoonTube&, Rotation&, ChildOf&, Harpoon&, LocalToWorld&, ChildOf&, Velocity&> query) {
            for (auto [ent, arrow, childOf0, tube, tubeRotation, childOf1, harpoon, harpoonLocalToWorld, childOf2, boatVelocity] : query)
            {
                if (harpoon.player == -1)
                {
                    continue;
                }

                if (inputs.justPressed("fire", harpoon.player))
                {
                    if (!harpoon.harpoonLoaded)
                        continue;

                    glm::vec3 harpoonPosition = harpoonLocalToWorld.worldPosition();
                    glm::quat harpoonRotation = harpoonLocalToWorld.worldRotation();

                    glm::vec3 forward = harpoonRotation * tubeRotation.quat * glm::vec3(-1.0F, 0.0F, 0.0F);

                    cmds.add(ent, Position{.vec = harpoonPosition + forward * 22.5F})
                        .add(ent, Rotation{.quat = harpoonRotation})
                        .add(ent, Scale{0.5F})
                        .add(ent, PhysicsBundle{
                            .mass = 1.0F, .velocity = boatVelocity.vec, .impulse = forward * harpoon.bulletSpeed})
                        .add(ent, Collider{})
                        .add(ent, BoxCollisionShape{.box = {.halfSize = {3.5F, 3.5F, 3.5F}}})
                        .add(ent, Bullet{})
                        .add(ent, Interpolated{harpoon.bulletScene});

                    harpoon.harpoonLoaded = false;
                }
            }
        });
}
