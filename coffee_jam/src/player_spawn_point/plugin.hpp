#pragma once

#include "../walker/plugin.hpp"

#include <glm/vec2.hpp>

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/assets/asset.hpp>
#include <cubos/engine/prelude.hpp>
#include <cubos/engine/scene/scene.hpp>
#include <cubos/engine/voxels/grid.hpp>

namespace demo
{
    /// @brief Component which identifies the point where the players will spawn at.
    struct PlayerSpawnPoint
    {
        CUBOS_REFLECT;

        struct Models
        {
            CUBOS_REFLECT;

            /// @brief Normal voxel model of the character.
            cubos::engine::Asset<cubos::engine::VoxelGrid> normal;

            /// @brief 'Holding' voxel model of the character.
            cubos::engine::Asset<cubos::engine::VoxelGrid> holding;
        };

        /// @brief Player scene.
        cubos::engine::Asset<cubos::engine::Scene> scene;

        /// @brief Scene root entity name.
        std::string root;

        /// @brief Scene camera entity name.
        std::string camera;

        /// @brief Models for each player.
        std::vector<Models> models;

        /// @brief Base player walker component.
        Walker walker;

        /// @brief Sub-entities which were spawned along with each player.
        std::vector<std::vector<cubos::engine::Entity>> subEntities{};
    };

    void playerSpawnPointPlugin(cubos::engine::Cubos& cubos);
} // namespace demo
