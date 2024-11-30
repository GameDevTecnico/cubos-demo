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

static const Asset<Scene> PlayerScene = AnyAsset("a88d914b-f1d7-463b-9a20-cf76bb5508d6");
static const Asset<InputBindings> PlayerBindingsAsset = AnyAsset("7b7463d1-e659-4167-91c3-27267952f071");

CUBOS_REFLECT_IMPL(airships::client::Player)
{
    return cubos::core::ecs::TypeBuilder<Player>("demo::Player")
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
                if (input.pressed("up"))
                {
                    move.z -= 1.0f;
                }
                if (input.pressed("down"))
                {
                    move.z += 1.0f;
                }
                if (input.pressed("left"))
                {
                    move.x -= 1.0f;
                }
                if (input.pressed("right"))
                {
                    move.x += 1.0f;
                }
                if (glm::length(move) > 0.0f)
                {
                    move = glm::normalize(move) * player.moveSpeed * dt.value();
                    pos.vec += move;
                    player.direction = glm::normalize(move);
                    // float angle = glm::atan(player.direction.x, player.direction.z);
                    // rot.quat = glm::angleAxis(angle, glm::vec3{0.0f, 1.0f, 0.0f});
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
                        }

                        CUBOS_INFO("Interacted with Interactable entity");
                    }
                }
            }
        });

    cubos.startupSystem("spawn player")
        .tagged(assetsTag)
        .call([](Commands cmds, const Assets& assets, Input& input, Query<Entity> all) {
            auto bp = cmds.spawn(assets.read(PlayerScene)->blueprint);
            bp.add("player", Position{glm::vec3{-5.0f, -7.0f, -20.0f}});
            // auto bp2 = cmds.spawn(assets.read(CarScene)->blueprint);
            // bp2.add("car", demo::Interactable{});
        });
}