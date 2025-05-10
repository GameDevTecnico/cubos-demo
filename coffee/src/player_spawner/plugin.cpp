#include "plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/input/plugin.hpp>
#include <cubos/engine/scene/scene.hpp>
#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/render/target/plugin.hpp>
#include <cubos/engine/render/target/target.hpp>
#include <cubos/engine/render/camera/plugin.hpp>
#include <cubos/engine/render/camera/camera.hpp>
#include <cubos/engine/render/camera/draws_to.hpp>

#include "../car/plugin.hpp"
#include "../follow/plugin.hpp"
#include "../round_manager/plugin.hpp"

using namespace cubos::engine;

static const Asset<Scene> CarSceneAssets[] = {
    AnyAsset("01bd8528-b53c-4df2-abd8-0162779a16e8"),
    AnyAsset("5ca206db-1bf7-4771-b9d6-dbf625b4e748"),
    AnyAsset("8c4939f8-9d3c-41e4-b015-298e8c5087ad"),
    AnyAsset("06f898f5-3b1c-435d-907f-4251afb72742"),
};
static const Asset<Scene> CameraSceneAsset = AnyAsset("78d434eb-276b-4c80-9046-0f285bc3ed54");

CUBOS_REFLECT_IMPL(coffee::Spawner)
{
    return cubos::core::ecs::TypeBuilder<Spawner>("coffee::Spawner")
        .withField("isChangeable", &Spawner::isChangeable)
        .withField("currentPlayers", &Spawner::currentPlayers)
        .build();
}

namespace coffee
{
    static void spawnPlayerCar(int player, Commands& cmds, Assets& assets)
    {
        // Spawn missing player.
        auto sceneRead = assets.read(CarSceneAssets[player % (sizeof(CarSceneAssets) / sizeof(Asset<Scene>))]);
        float side = (player % 2 == 0) ? -1 : 1;
        float offset = (player < 3) ? 1 : 3;
        auto builder = cmds.spawn(*sceneRead)
                           .add(PlayerOwner{.player = player, .canMove = false})
                           .add(Position{.vec = {side * offset * 5.0F, 30.0F, -5.0F}})
                           .named("car" + std::to_string(player));
    }

    static void spawnPlayerCameras(Commands& cmds, Assets& assets, Query<Entity, PlayerOwner&> cars,
                                   Query<Entity, const RenderTarget&> renderTargets,
                                   Query<Entity, Camera&, Position&, Rotation&> cameras)
    {
        // destroy initial camera, we only have one so this works
        for (auto [ent, camera, position, rotation] : cameras)
        {
            cmds.destroy(ent);
        }

        // create new follow cameras for each spawned car
        for (auto [carEnt, playerOwner] : cars)
        {
            playerOwner.canMove = true;

            auto sceneRead = assets.read(CameraSceneAsset);
            auto builder = cmds.spawn(*sceneRead).named("camera" + std::to_string(playerOwner.player));

            auto cameraEnt = builder.entity();

            cmds.relate(cameraEnt, carEnt,
                        Follow{.distance = 25.0F, .height = 7.5, .halfTime = 0.25, .rotationHalfTime = 0.05});

            auto [renderTargetEnt, renderTarget] = *renderTargets.first();
            cmds.relate(cameraEnt, renderTargetEnt, DrawsTo{});
        }
    }
} // namespace coffee

void coffee::playerSpawnerPlugin(Cubos& cubos)
{
    cubos.depends(transformPlugin);
    cubos.depends(inputPlugin);
    cubos.depends(carPlugin);
    cubos.depends(assetsPlugin);
    cubos.depends(renderTargetPlugin);
    cubos.depends(cameraPlugin);
    cubos.depends(roundManagerPlugin);

    cubos.resource<Spawner>();

    cubos.system("read input and choose number of cars")
        .call([](Commands cmds, Assets& assets, Input& input, Spawner& spawner, GameRoundSettings& roundSettings,
                 Query<RoundManager&, WaitingRoundStart&> waitingState) {
            if (roundSettings.currentRound == 0)
            {
                if (!spawner.hasStarted)
                {
                    if (!spawner.isChangeable)
                    {
                        spawner.isChangeable = true;
                        spawner.currentPlayers = 0;
                    }
                    if (spawner.currentPlayers == 0 && input.gamepadCount() > 0)
                    {
                        input.gamepad(1, 0);
                    }
                    else if (spawner.currentPlayers > 1 && input.gamepadCount() > 0)
                    {
                        int freeGamepads = input.gamepadCount();
                        for (int i = spawner.currentPlayers; i >= 1; --i)
                        {
                            if (freeGamepads > 0)
                            {
                                input.gamepad(i, freeGamepads - 1);
                                freeGamepads--;
                            }
                            else
                            {
                                input.gamepad(i, -1);
                            }
                        }
                    }

                    if (input.justPressed("handbrake", 1) || input.justPressed("handbrake", 2) ||
                        input.justPressed("handbrake", 3) ||
                        input.justPressed("handbrake", 4)) // potentially add all others here
                    {
                        if (spawner.currentPlayers < 4)
                        {
                            spawner.currentPlayers++;
                            spawnPlayerCar(spawner.currentPlayers, cmds, assets);
                        }
                    }
                }
            }
            else
            {
                if (spawner.canSpawnCar && waitingState.count() == 1)
                {
                    if (spawner.carsSpawned >= spawner.currentPlayers)
                    {
                        spawner.canSpawnCar = false;
                        return;
                    }

                    auto match = waitingState.first();
                    if (!match)
                    {
                        return;
                    }
                    auto& [manager, waitingRoundStart] = *match;

                    if (waitingRoundStart.time >= spawner.timeBetweenSpawns * spawner.carsSpawned)
                    {
                        spawner.carsSpawned++;
                        spawnPlayerCar(spawner.carsSpawned, cmds, assets);
                    }
                }
            }
        });

    //
    cubos.system("read input, spawn cameras and begin game")
        .before(transformUpdateTag)
        .call([](Commands cmds, Assets& assets, Input& input, Spawner& spawner, GameRoundSettings& roundSettings,
                 Query<Entity, PlayerOwner&> cars, Query<Entity, const RenderTarget&> renderTargets,
                 Query<Entity, Camera&, Position&, Rotation&> cameras, Query<RoundPlaying&> playingState,
                 const DeltaTime& dt) {
            if (spawner.currentPlayers <= 0)
            {
                return;
            }

            if (roundSettings.currentRound == 0)
            {
                if (spawner.hasStarted && playingState.empty())
                {
                    spawner.timeSinceStart += dt.value();
                    if (spawner.timeSinceStart >= spawner.timeToStart)
                    {
                        spawner.hasStarted = false;
                        spawner.timeSinceStart = 0.0F;
                        spawnPlayerCameras(cmds, assets, cars, renderTargets, cameras);
                        cmds.remove<WaitingRoundStart>(roundSettings.roundManagerEntity);
                        cmds.add(roundSettings.roundManagerEntity, RoundPlaying{});
                    }
                }
                else if (input.justPressed("play", 1) || input.justPressed("play", 2) || input.justPressed("play", 3) ||
                         input.justPressed("play", 4))
                {
                    spawner.hasStarted = true;
                    spawner.canSpawn = false;
                    spawner.isChangeable = false;
                }
            }
            else
            {
                if (spawner.canSpawn && playingState.count() != 0)
                {
                    spawner.canSpawn = false;
                    spawnPlayerCameras(cmds, assets, cars, renderTargets, cameras);
                }
            }
        });

    cubos.observer("spawn cameras on round playing")
        .onAdd<WaitingRoundStart>()
        .call([](Spawner& spawner, Query<WaitingRoundStart&> query) {
            spawner.canSpawnCar = true;
            spawner.carsSpawned = 0;
            spawner.canSpawn = true;
        });
}