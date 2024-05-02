#include "plugin.hpp"
#include "../progression/plugin.hpp"
#include "../player_controller/plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/external/vector.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/settings/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/renderer/plugin.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(demo::PlayerSpawnPoint::Models)
{
    return cubos::core::ecs::TypeBuilder<PlayerSpawnPoint::Models>("demo::PlayerSpawnPoint::Models")
        .withField("normal", &PlayerSpawnPoint::Models::normal)
        .withField("holding", &PlayerSpawnPoint::Models::holding)
        .build();
}

CUBOS_REFLECT_IMPL(demo::PlayerSpawnPoint)
{
    return cubos::core::ecs::TypeBuilder<PlayerSpawnPoint>("demo::PlayerSpawnPoint")
        .withField("scene", &PlayerSpawnPoint::scene)
        .withField("root", &PlayerSpawnPoint::root)
        .withField("camera", &PlayerSpawnPoint::camera)
        .withField("models", &PlayerSpawnPoint::models)
        .withField("walker", &PlayerSpawnPoint::walker)
        .build();
}

void demo::playerSpawnPointPlugin(Cubos& cubos)
{
    cubos.depends(settingsPlugin);
    cubos.depends(assetsPlugin);
    cubos.depends(rendererPlugin);
    cubos.depends(transformPlugin);
    cubos.depends(walkerPlugin);
    cubos.depends(progressionPlugin);
    cubos.depends(playerControllerPlugin);

    cubos.component<PlayerSpawnPoint>();

    cubos.system("spawn missing players during the day")
        .call([](Commands cmds, Assets& assets, Settings& settings, const Progression& progression,
                 ActiveCameras& activeCameras, Query<PlayerSpawnPoint&, const ChildOf&, Entity> spawnPoints,
                 Query<Entity, const PlayerController&> playerControllers) {
            auto match = spawnPoints.first();
            if (!match)
            {
                return;
            }

            auto [spawnPoint, _1, mapEnt] = *match;
            auto playerCount = settings.getInteger("player.count", 1);

            if (playerCount < 1)
            {
                CUBOS_WARN("Invalid player count {}, setting to 1", playerCount);
                playerCount = 1;
                settings.setInteger("player.count", playerCount);
            }
            else if (playerCount > spawnPoint.models.size())
            {
                CUBOS_WARN("Not enough models for {} players, limiting to {}", playerCount, spawnPoint.models.size());
                playerCount = spawnPoint.models.size();
                settings.setInteger("player.count", playerCount);
            }

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

            if (progression.timeOfDay >= progression.dayDuration)
            {
                // Do not spawn players during the night.
                return;
            }

            for (int player = 1; player <= playerCount; player++)
            {
                if (!playerSpawned[player - 1])
                {
                    if (spawnPoint.subEntities.size() < player)
                    {
                        spawnPoint.subEntities.resize(player, {});
                    }

                    // Destroy any remaining sub-entities from a previous life.
                    for (auto subEnt : spawnPoint.subEntities[player - 1])
                    {
                        cmds.destroy(subEnt);
                    }
                    spawnPoint.subEntities[player - 1].clear();

                    // Spawn missing player.
                    auto sceneRead = assets.read(spawnPoint.scene);
                    auto builder = cmds.spawn(sceneRead->blueprint);

                    auto playerEnt = builder.entity(spawnPoint.root);
                    cmds.relate(playerEnt, mapEnt, ChildOf{})
                        .add(playerEnt, spawnPoint.walker)
                        .add(playerEnt, PlayerController{.player = player,
                                                         .normal = spawnPoint.models[player - 1].normal,
                                                         .holding = spawnPoint.models[player - 1].holding});
                    activeCameras.entities[player - 1] = builder.entity(spawnPoint.camera);

                    for (auto& [_, name] : sceneRead->blueprint.entities())
                    {
                        spawnPoint.subEntities[player - 1].push_back(builder.entity(name));
                    }
                }
            }
        });
}
