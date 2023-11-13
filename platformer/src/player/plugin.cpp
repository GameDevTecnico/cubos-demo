#include <cubos/core/ecs/system/query.hpp>

#include <cubos/engine/renderer/environment.hpp>
#include <cubos/engine/renderer/directional_light.hpp>
#include <cubos/engine/renderer/plugin.hpp>
#include <cubos/engine/settings/settings.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/voxels/plugin.hpp>
#include <cubos/engine/input/plugin.hpp>
#include <cubos/engine/scene/plugin.hpp>
#include <cubos/engine/physics/plugin.hpp>

#include <vector>
#include <cmath>

#include "player.hpp"
#include "plugin.hpp"

using cubos::core::ecs::Commands;
using cubos::core::ecs::Query;
using cubos::core::ecs::Read;
using cubos::core::ecs::Write;

using namespace cubos::engine;

using demo::Player;

static const Asset<Scene> PlayerSceneAsset = AnyAsset("931545f5-6c1e-43bf-bb1d-ba2c1f6e9333");
static const Asset<InputBindings> Player0BindingsAsset = AnyAsset("bf49ba61-5103-41bc-92e0-8a442d7842c3");
static const Asset<InputBindings> Player1BindingsAsset = AnyAsset("bf49ba61-5103-41bc-92e0-8a442d7842c4");

static void loadInputBindings(Read<Assets> assets, Write<Input> input)
{
    input->bind(*assets->read<InputBindings>(Player0BindingsAsset), 0);
    input->bind(*assets->read<InputBindings>(Player1BindingsAsset), 1);
}

// create method to assign cameras
// change stuff

static void spawnPlayers(Commands cmds, Write<Assets> assets, Write<ActiveCameras> activeCameras)
{
    auto player = assets->read(PlayerSceneAsset);
    cmds.spawn(player->blueprint).add("player", Player{0});
    cmds.spawn(player->blueprint).add("player", Player{1}, Position{{50.0F, 0.0F, 0.0F}});

    // Spawn the camera entity.
    activeCameras->entities[0] =
        cmds.create()
            .add(Camera{60.0F, 0.1F, 1000.0F})
            .add(Position{{0.0F, 10.0F, 100.0F}})
            .add(Rotation{glm::quatLookAt(glm::normalize(glm::vec3{0.0F, 0.0F, -1.0F}), glm::vec3{0.0F, 1.0F, 0.0F})})
            .entity();

    // Spawn the camera entity.
    /*
    activeCameras->entities[1] =
        cmds.create()
            .add(Camera{60.0F, 0.1F, 1000.0F})
            .add(Position{{0.0F, 120.0F, -200.0F}})
            .add(Rotation{glm::quatLookAt(glm::normalize(glm::vec3{0.0F, -1.0F, 1.0F}), glm::vec3{0.0F, 1.0F, 0.0F})})
            .entity();
    */

    /*
    add(FollowEntity{.entityToFollow = entity1,
                              .positionOffset = {0.0f, 15.0f, -40.0f},
                              .rotationOffset = glm::angleAxis(3.1415f, glm::vec3(0.0F, 1.0F, 0.0F)) *
                                                glm::angleAxis(-0.2618f, glm::vec3(1.0f, 0.0f, 0.0f))})
    */
}

static void move(Query<Write<Player>, Write<PhysicsVelocity>, Write<Rotation>> query, Read<Input> input,
                 Read<DeltaTime> deltaTime, Write<Settings> settings)
{
    for (auto [entity, player, velocity, rotation] : query)
    {
        const float force = settings->getDouble("force", 5000.0F);
        const float jumpForce = settings->getDouble("jumpForce", 4000.0F);
        const float dragForce = settings->getDouble("dragForce", -2000.0F);
        const float rotationSpeed = settings->getDouble("rotationSpeed", 0.02F);
        const float maxSpeed = settings->getDouble("maxSpeed", 110.0F);

        auto moveForce = input->axis("move", player->id) * force;
        auto turn = -input->axis("turn", player->id) * rotationSpeed;

        float speed = glm::length(velocity->velocity);
        if (speed > maxSpeed)
        {
            auto direction = glm::normalize(velocity->velocity);
            velocity->velocity = direction * maxSpeed;
        }

        auto jump = input->pressed("jump", player->id);

        auto rotationDelta = glm::angleAxis(turn, glm::vec3{0.0F, 1.0F, 0.0F});
        rotation->quat = rotationDelta * rotation->quat;

        glm::vec3 forward = rotation->quat * glm::vec3(0.0f, 0.0f, 1.0f);
        glm::vec3 side = rotation->quat * glm::vec3(1.0f, 0.0f, 0.0f);

        if (glm::length(side) > 3.0F)
        {
            velocity->force += side * dragForce;
        }
        if (moveForce == 0.0F && glm::length(forward) > 3.0F)
        {
            velocity->force += forward * dragForce;
        }
        velocity->force += forward * moveForce;
        // velocity->force += glm::vec3{0.0F, 1.0F, 0.0F} * 600.0F;

        if (jump && player->isOnGround)
        {
            velocity->impulse += glm::vec3{0.0F, 1.0F, 0.0F} * jumpForce;
            player->isOnGround = false;
        }
    }
}

// handle ground collision
// handle dead

// respawn player

void demo::playersPlugin(Cubos& cubos)
{
    cubos.addPlugin(rendererPlugin);
    cubos.addPlugin(voxelsPlugin);
    cubos.addPlugin(inputPlugin);
    cubos.addPlugin(physicsPlugin);

    cubos.addComponent<Player>();

    cubos.startupSystem(loadInputBindings).tagged("cubos.assets");
    cubos.startupSystem(spawnPlayers).tagged("cubos.assets");
    cubos.system(move).tagged("player.move").tagged("cubos.physics.apply_forces").before("cubos.transform.update");
}
