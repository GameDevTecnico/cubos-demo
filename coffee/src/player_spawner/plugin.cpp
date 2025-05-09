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
// #include <cubos/engine/render/split_screen/plugin.hpp>

#include "../car/plugin.hpp"
#include "../follow/plugin.hpp"

using namespace cubos::engine;

static const Asset<Scene> CarSceneAsset = AnyAsset("f91c4d18-7a4e-4269-aef2-3c9987d7df1e");
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
        CUBOS_INFO("Spawner player {}", player);

        // Spawn missing player.
        auto sceneRead = assets.read(CarSceneAsset);
        float side = (player % 2 == 0) ? -1 : 1;
        float offset = (player < 3) ? 1 : 3;
        auto builder = cmds.spawn(*sceneRead)
                           .add(PlayerOwner{.player = player, .canMove = false})
                           .add(Position{.vec = {side * offset * 5.0F, 30.0F, -5.0F}})
                           .named("car" + std::to_string(player));
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

    cubos.resource<Spawner>();

    cubos.system("read input and choose number of cars")
        .call([](Commands cmds, Assets& assets, Input& input, Spawner& spawner) {
            if (!spawner.isChangeable)
            {
                return;
            }
            if (input.justPressed("shoot", 1) || input.justPressed("shoot", 2) || input.justPressed("shoot", 3) ||
                input.justPressed("shoot", 4)) // potentially add all others here
            {
                if (spawner.currentPlayers < 4)
                {
                    spawner.currentPlayers++;
                    spawnPlayerCar(spawner.currentPlayers, cmds, assets);
                }
            }
        });

    cubos.system("read input, spawn cameras and begin game")
        .before(transformUpdateTag)
        .call([](Commands cmds, Assets& assets, Input& input, Spawner& spawner, Query<Entity, PlayerOwner&> cars,
                 Query<Entity, const RenderTarget&> renderTargets,
                 Query<Entity, Camera&, Position&, Rotation&> cameras) {
            if (!spawner.canSpawn)
            {
                return;
            }

            // read input and end player choice
            if (input.justPressed("play", 1) || input.justPressed("play", 2) || input.justPressed("play", 3) ||
                input.justPressed("play", 4))
            {
                spawner.canSpawn = false;
                spawner.isChangeable = false;

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
        });
}