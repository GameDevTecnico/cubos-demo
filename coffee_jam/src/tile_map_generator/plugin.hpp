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
        int mapSide = 64;

        /// @brief How many tiles are in the side of a single chunk.
        int chunkSide = 8;

        /// @brief How many voxels wide is a single tile.
        int tileSide = 8;

        /// @brief How many tiles are used as a border around the map.
        int border = 8;

        /// @name Voxel grids for the different floor types.
        /// @{
        cubos::engine::Asset<cubos::engine::VoxelGrid> grass;
        cubos::engine::Asset<cubos::engine::VoxelGrid> roadDash;
        cubos::engine::Asset<cubos::engine::VoxelGrid> roadLine;
        cubos::engine::Asset<cubos::engine::VoxelGrid> roadCorner;
        cubos::engine::Asset<cubos::engine::VoxelGrid> roadCurve;
        cubos::engine::Asset<cubos::engine::VoxelGrid> roadTJunction;
        cubos::engine::Asset<cubos::engine::VoxelGrid> roadJunction;
        cubos::engine::Asset<cubos::engine::VoxelGrid> roadSimple;
        cubos::engine::Asset<cubos::engine::VoxelGrid> sidewalk;
        /// @}

        /// @name Voxel grids for the different wall types.
        /// @{
        cubos::engine::Asset<cubos::engine::VoxelGrid> fenceStraight;
        cubos::engine::Asset<cubos::engine::VoxelGrid> fenceCurve;
        cubos::engine::Asset<cubos::engine::VoxelGrid> wallStraight;
        cubos::engine::Asset<cubos::engine::VoxelGrid> wallCurve;
        /// @}

        /// @name Scenes for the different objects which may be lying around.
        /// @{
        cubos::engine::Asset<cubos::engine::Scene> crate;
        cubos::engine::Asset<cubos::engine::Scene> car1;
        /// @}
    };

    void tileMapGeneratorPlugin(cubos::engine::Cubos& cubos);
} // namespace demo
