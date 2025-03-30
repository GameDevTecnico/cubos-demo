#include "plugin.hpp"
#include "../player_controller/plugin.hpp"
#include "../health/plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/external/vector.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/settings/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/input/plugin.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(demo::PlayerSpawner::Player)
{
    return cubos::core::ecs::TypeBuilder<PlayerSpawner::Player>("demo::PlayerSpawner::Player")
        .withField("bindings", &PlayerSpawner::Player::bindings)
        .withField("scene", &PlayerSpawner::Player::scene)
        .withField("needsGamepad", &PlayerSpawner::Player::needsGamepad)
        .withField("team", &PlayerSpawner::Player::team)
        .build();
}

CUBOS_REFLECT_IMPL(demo::PlayerSpawner)
{
    return cubos::core::ecs::TypeBuilder<PlayerSpawner>("demo::PlayerSpawner")
        .withField("players", &PlayerSpawner::players)
        .withField("movement", &PlayerSpawner::movement)
        .withField("controller", &PlayerSpawner::controller)
        .build();
}



void demo::playerSpawnerPlugin(Cubos& cubos)
{
    cubos.depends(settingsPlugin);
    cubos.depends(assetsPlugin);
    cubos.depends(transformPlugin);
    cubos.depends(inputPlugin);
    cubos.depends(movementPlugin);
    cubos.depends(playerControllerPlugin);

    cubos.component<PlayerSpawner>();

    cubos.system("spawn players")
        .call([](Commands cmds, Assets& assets, Settings& settings, Input& input,
                 Query<PlayerSpawner&, const ChildOf&, Entity> spawners,
                 Query<Entity, const PlayerController&> playerControllers) {
            auto match = spawners.first();
            if (!match)
            {
                return;
            }

            auto [spawner, _1, mapEnt] = *match;
            auto playerCount = settings.getInteger("player.count", 2);

            if (playerCount < 1)
            {
                CUBOS_WARN("Invalid player count {}, setting to 1", playerCount);
                playerCount = 1;
                settings.setInteger("p layer.count", playerCount);
            }
            else if (playerCount > spawner.players.size())
            {
                CUBOS_WARN("Not enough assets for {} players, limiting to {}", playerCount, spawner.players.size());
                playerCount = spawner.players.size();
                settings.setInteger("player.count", playerCount);
            }

            // Check if there are enough gamepads for all players.
            int gamepadCount = input.gamepadCount();
            for (int player = 1; player <= playerCount; player++)
            {
                if (spawner.players[player - 1].needsGamepad)
                {
                    gamepadCount -= 1;
                }
            }
            if (gamepadCount < 0)
            {
                CUBOS_WARN("Not enough gamepads for {} players, keeping only {} players", playerCount,
                           playerCount + gamepadCount);
                playerCount += gamepadCount;
            }
            gamepadCount = input.gamepadCount();

            std::vector<bool> playerSpawned(playerCount, false);
            for (auto [playerEnt, controller] : playerControllers)
            {
                if (controller.player > playerCount)
                {
                    // Remove additional players.
                    cmds.destroy(playerEnt);
                }
                else
                {
                    playerSpawned[controller.player - 1] = true;
                }
            }

            for (int player = 1; player <= playerCount; player++)
            {
                if (!playerSpawned[player - 1])
                {
                    CUBOS_WARN("SPAWNED PLAYER {}", player);

                    // Spawn missing player.
                    auto sceneRead = assets.read(spawner.players[player - 1].scene);
                    auto builder = cmds.spawn(*sceneRead);

                    auto controller = spawner.controller;
                    controller.player = player;

                    auto playerEnt = builder.entity();
                    
                    cmds.relate(playerEnt, mapEnt, ChildOf{})
                        .add(playerEnt, spawner.movement)
                        .add(playerEnt, controller)
                        .add(playerEnt, demo::Health{.hp = 3, .team = spawner.players[player - 1].team});

                    // Bind input for the player.
                    input.bind(*assets.read(spawner.players[player - 1].bindings), player);
                }

                // Assign a gamepad to the player.
                int gamepad = -1;
                if (gamepadCount < playerCount)
                {
                    if (spawner.players[player - 1].needsGamepad)
                    {
                        gamepad = 0;
                        for (int i = 1; i < player; ++i)
                        {
                            if (spawner.players[i - 1].needsGamepad)
                            {
                                gamepad += 1;
                            }
                        }
                    }
                    else
                    {
                        gamepad = 0;
                        for (int i = 1; i <= playerCount; ++i)
                        {
                            if (spawner.players[i - 1].needsGamepad || i < player)
                            {
                                gamepad += 1;
                            }
                        }
                        if (gamepad >= gamepadCount)
                        {
                            gamepad = -1;
                        }
                    }
                }
                else
                {
                    gamepad = player - 1;
                }

                if (input.gamepad(player) != gamepad)
                {
                    input.gamepad(player, gamepad);
                }
            }
        });
}
