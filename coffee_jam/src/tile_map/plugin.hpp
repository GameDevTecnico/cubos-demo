#pragma once

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/assets/asset.hpp>
#include <cubos/engine/voxels/grid.hpp>
#include <cubos/engine/prelude.hpp>

#include <unordered_map>
#include <vector>
#include <string>

namespace demo
{
    /// @brief Component which keeps information about the tile-map.
    ///
    /// On addition, immediately creates entities for the tile chunks.
    struct TileMap
    {
        CUBOS_REFLECT;

        /// @brief How many tiles are in the side of a single chunk.
        int chunkSide = 8;

        /// @brief How many voxels wide is a single tile.
        int tileSide = 8;

        /// @brief Map of tile indices to their voxel grids.
        std::vector<cubos::engine::Asset<cubos::engine::VoxelGrid>> types;

        /// @brief Tile-map data. Each dimension's size must be a multiple of @ref chunkSide.
        std::vector<std::vector<unsigned char>> tiles;

        /// @brief Entity occupying each tile (if any).
        std::vector<std::vector<cubos::engine::Entity>> entities;
    };

    void tileMapPlugin(cubos::engine::Cubos& cubos);
} // namespace demo
