#pragma once

#include "../player_movement/plugin.hpp"
#include "../health/plugin.hpp"

#include <glm/vec2.hpp>

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/assets/asset.hpp>
#include <cubos/engine/prelude.hpp>
#include <cubos/engine/scene/scene.hpp>
#include <cubos/engine/voxels/grid.hpp>
#include <cubos/engine/input/bindings.hpp>

namespace demo
{
    /// @brief Component which identifies the point where the players will spawn at.
    struct PlayerSpawner
    {
        CUBOS_REFLECT;

        struct Player
        {
            CUBOS_REFLECT;

            /// @brief Input bindings for the player.
            cubos::engine::Asset<cubos::engine::InputBindings> bindings;

            /// @brief Player scene.
            cubos::engine::Asset<cubos::engine::Scene> scene;

            /// @brief Does the player need a gamepad?
            bool needsGamepad = true;

            /// @brief Player team
            demo::Team team;
        };

        /// @brief Data for each player.
        std::vector<Player> players;

        /// @brief Base player movement component.
        Movement movement;
    };

    void playerSpawnerPlugin(cubos::engine::Cubos& cubos);
} // namespace demo
