#pragma once

#include "../walker/plugin.hpp"

#include <glm/vec2.hpp>

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/assets/asset.hpp>
#include <cubos/engine/prelude.hpp>
#include <cubos/engine/scene/scene.hpp>

namespace demo
{
    /// @brief Component which spawns waves of zombies during the night.
    struct Waves
    {
        CUBOS_REFLECT;

        /// @brief Base number of zombies to spawn.
        int waveZombiesBase = 3;

        /// @brief Increase in wave zombies per night.
        int waveZombiesIncrease = 2;

        /// @brief Number of waves per night.
        int wavesPerNight = 3;

        /// @brief Zombie scene to spawn.
        cubos::engine::Asset<cubos::engine::Scene> zombie;

        /// @brief Zombie scene root entity name.
        std::string zombieRoot;

        /// @brief Base zombie walker component.
        Walker walker;
    };

    void wavesPlugin(cubos::engine::Cubos& cubos);
} // namespace demo
