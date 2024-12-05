#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/render/voxels/grid.hpp>
#include <cubos/engine/render/voxels/load.hpp>
#include <cubos/engine/render/voxels/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/input/plugin.hpp>
#include <cubos/engine/collisions/plugin.hpp>
#include <glm/vec3.hpp>
#include <cubos/engine/collisions/raycast.hpp>
#include <cubos/engine/scene/scene.hpp>
#include <map>

#include "plugin.hpp"
#include "../interactable/plugin.hpp"
#include "../follow/plugin.hpp"

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(airships::client::Player)
{
    return cubos::core::ecs::TypeBuilder<Player>("airships::client::Player")
        .withField("player", &Player::player)
        .withField("horizontalAxis", &Player::horizontalAxis)
        .withField("verticalAxis", &Player::verticalAxis)
        .withField("interactAction", &Player::interactAction)
        .withField("canMove", &Player::canMove)
        .withField("direction", &Player::direction)
        .withField("moveSpeed", &Player::moveSpeed)
        .build();
}

void airships::client::playerPlugin(Cubos& cubos)
{
    cubos.depends(assetsPlugin);
    cubos.depends(transformPlugin);
    cubos.depends(renderVoxelsPlugin);
    cubos.depends(inputPlugin);
    cubos.depends(collisionsPlugin);
    cubos.depends(interactablePlugin);
    cubos.depends(followPlugin);

    cubos.component<Player>();

    cubos.system("do Player movement")
        .call([](Commands cmds, Query<LocalToWorld&, const Follow&, Player&, Position&, Rotation&, const ChildOf&, const LocalToWorld&> players,
                 const DeltaTime& dt, const Input& input) {
            for (auto [cameraLTW, follow, player, pos, rot, childOf, boatLTW] : players)
            {
                if (player.player == -1 || !player.canMove)
                {
                    continue;
                }

                // Get the directional vectors from the camera's rotation.
                // We need to translate them to the player's parent space.
                auto forward4 = boatLTW.inverse() * cameraLTW.mat * glm::vec4{0.0F, 0.0F, -1.0F, 0.0F};
                glm::vec3 forward = {forward4.x, forward4.y, forward4.z};
                forward.y = 0.0F;
                forward = glm::normalize(forward);
                auto right = glm::cross(forward, {0.0F, 1.0F, 0.0F});

                // Get the movement vector from the directions above and the input
                glm::vec3 moveDirection = {0.0f, 0.0f, 0.0f};
                moveDirection.x = input.axis(player.horizontalAxis.c_str(), player.player);
                moveDirection.z = input.axis(player.verticalAxis.c_str(), player.player);
                moveDirection = moveDirection.x * right + moveDirection.z * forward;
                if (glm::length2(moveDirection) == 0.0F)
                {
                    continue;
                }
                moveDirection = glm::normalize(moveDirection);

                // Update the player position and rotation
                pos.vec += moveDirection * player.moveSpeed * dt.value();
                rot.quat = glm::quatLookAt(-moveDirection, {0.0F, 1.0F, 0.0F});
            }
        });

    cubos.system("do Player interaction")
        .call([](Commands cmds, Query<Entity, Player&, const LocalToWorld&> players,
                 Query<const Interactable&> interactables, const Assets& assets, Raycast raycast, const Input& input) {
            for (auto [playerEnt, player, playerLTW] : players)
            {
                if (input.justPressed(player.interactAction.c_str(), player.player))
                {
                    if (auto hit = raycast.fire({playerLTW.worldPosition(), playerLTW.forward()}))
                    {
                        if (interactables.at(hit->entity))
                        {
                            cmds.add(hit->entity, Interaction{.player = playerEnt});
                        }
                    }
                }
            }
        });
}