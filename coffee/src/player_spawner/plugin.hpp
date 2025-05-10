#pragma once

#include <cubos/core/reflection/reflect.hpp>
#include <glm/vec3.hpp>
#include <cubos/engine/prelude.hpp>

namespace coffee
{
    struct Spawner
    {
        CUBOS_REFLECT;

        bool canSpawn = true;
        bool canSpawnCar = true;
        int carsSpawned = 0;
        float timeBetweenSpawns = 0.6F;
        bool isChangeable = true;
        int currentPlayers = 0; // press once per player
        bool hasStarted = false;
    };

    void playerSpawnerPlugin(cubos::engine::Cubos& cubos);
} // namespace coffee
