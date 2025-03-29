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
    /// @brief Tile data.
    struct Tile
    {
        CUBOS_REFLECT;

        /// @brief Index of the tile's grid in the corresponding tile type vector.
        unsigned char type;

        /// @brief Rotation of the tile in 90-degree increments.
        unsigned char rotation;

        /// @brief Height of the tile.
        int height;
    };

    /// @brief Component which keeps information about the tile-map.
    ///
    /// On addition, immediately creates entities for the tile chunks.
    struct TileMap
    {
        CUBOS_REFLECT;

        /// @brief How many voxels wide is a single tile.
        int tileSide = 8;

        /// @brief How many voxels high is a single tile.
        int tileHeight = 8;

        /// @brief Map of tile indices to their voxel grids.
        std::vector<cubos::engine::Asset<cubos::engine::VoxelGrid>> types;

        /// @brief Tile-map data.
        std::vector<std::vector<Tile>> tiles;

        /// @brief Entity occupying each tile (if any).
        std::vector<std::vector<cubos::engine::Entity>> entities;
    };

    void tileMapPlugin(cubos::engine::Cubos& cubos);
} // namespace demo
