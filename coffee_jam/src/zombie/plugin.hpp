#pragma once

#include <string>

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/prelude.hpp>
#include <cubos/engine/assets/asset.hpp>
#include <cubos/engine/assets/assets.hpp>
#include <cubos/engine/voxels/grid.hpp>

namespace demo
{
    /// @brief Component used to allow zombie movement.
    struct ZombieController
    {
        CUBOS_REFLECT;

        /// @brief Axis used to move the character in the x direction.
        std::string moveX = "move-x";

        /// @brief Axis used to move the character in the y direction.
        std::string moveY = "move-y";

        /// @brief Normal voxel model of the character.
        cubos::engine::Asset<cubos::engine::VoxelGrid> normal;
    };

    void zombiePlugin(cubos::engine::Cubos& cubos);
} // namespace demo
