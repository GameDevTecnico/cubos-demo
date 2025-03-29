#pragma once

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/assets/asset.hpp>
#include <cubos/engine/voxels/grid.hpp>
#include <cubos/engine/scene/scene.hpp>
#include <cubos/engine/prelude.hpp>

#include <unordered_map>
#include <vector>
#include <string>

namespace demo
{
    /// @brief Component which generates a TileMap component and removes itself.
    struct TileMapGenerator
    {
        CUBOS_REFLECT;

        /// @brief How many tiles are in the side of the entire map.
        int mapSide = 16;

        /// @brief How many voxels wide is a single tile.
        int tileSide = 8;

        /// @brief How many voxels high is a single wall tile.
        int tileHeight = 8;

        /// @name Voxel grids for the different floor types.
        /// @{
        cubos::engine::Asset<cubos::engine::VoxelGrid> grass;
        cubos::engine::Asset<cubos::engine::VoxelGrid> sand;
        /// @}
    };

    void tileMapGeneratorPlugin(cubos::engine::Cubos& cubos);
} // namespace demo
