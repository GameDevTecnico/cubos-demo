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

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(airships::client::Player)
{
    return cubos::core::ecs::TypeBuilder<Player>("airships::client::Player")
        .withField("player", &Player::player)
        .withField("horizontalAxis", &Player::horizontalAxis)
        .withField("verticalAxis", &Player::verticalAxis)
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

    cubos.component<Player>();

    // Plugin para mover o jogador
    cubos.system("move player")
        .call([](Commands cmds, Query<Player&, Position&, Rotation&> players, const DeltaTime& dt, const Input& input) {
            for (auto [player, pos, rot] : players)
            {
                glm::vec3 move = {0.0f, 0.0f, 0.0f};
                move.x = -input.axis(player.horizontalAxis.c_str(), player.player);
                move.z = input.axis(player.verticalAxis.c_str(), player.player);
                if (glm::length(move) > 0.0f)
                {
                    move = glm::normalize(move) * player.moveSpeed * dt.value();
                    pos.vec += move;
                    player.direction = glm::normalize(move);
                    float angle = glm::atan(player.direction.x, player.direction.z);
                    rot.quat = glm::angleAxis(angle, glm::vec3{0.0f, 1.0f, 0.0f});
                }
            }
        });

    cubos.system("interaction with raycast")
        .call([](Query<Player&, Position&> players, Query<Interactable&, Position&> interactables, const Assets& assets,
                 Raycast raycast, const Input& input) {
            for (auto [player, playerPos] : players)
            {
                if (auto hit = raycast.fire({playerPos.vec, player.direction}))
                {
                    if (interactables.at(hit->entity))
                    {
                        if (input.pressed("interact"))
                        {
                            // DO SOMETHING
                            CUBOS_INFO("Interacted with Interactable entity");
                        }
                    }
                }
            }
        });
}