#include <cubos/engine/transform/position.hpp>
#include <cubos/engine/scene/scene.hpp>
#include <cubos/engine/input/input.hpp>
#include <cubos/engine/assets/assets.hpp>

#include "../player/player.hpp"
#include "../orbit_camera/controller.hpp"
#include "plugin.hpp"
#include "spawn.hpp"

using cubos::core::ecs::Commands;
using cubos::core::ecs::Query;
using cubos::core::ecs::Read;
using cubos::core::ecs::Write;

using namespace cubos::engine;

using namespace demo;

static void spawnSystem(Commands cmds, Query<Read<Player>> players, Query<Read<Spawn>, Read<Position>> spawns,
                        Write<Assets> assets, Write<Input> input)
{
    for (auto [spawnEnt, spawn, position] : spawns)
    {
        // Check if the player matching this spawn exists
        bool foundPlayer = false;
        for (auto [playerEnt, player] : players)
        {
            if (player->id == spawn->playerId)
            {
                foundPlayer = true;
                break;
            }
        }

        // If it does, do nothing
        if (foundPlayer)
        {
            continue;
        }

        // Otherwise, spawn it
        auto builder = cmds.spawn(assets->read(Asset<Scene>("931545f5-6c1e-43bf-bb1d-ba2c1f6e9333"))->blueprint);
        builder.get<Player>("player").id = spawn->playerId;
        builder.get<Position>("player") = *position;
        builder.get<OrbitCameraController>("camera").playerId = spawn->playerId;
        input->bind(*assets->read(spawn->bindings), spawn->playerId);
    }
}

void demo::spawnPlugin(Cubos& cubos)
{
    cubos.addComponent<Spawn>();
    cubos.system(spawnSystem);
}
