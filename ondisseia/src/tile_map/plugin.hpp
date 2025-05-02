#pragma once

#include "../player_controller/plugin.hpp"

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/assets/asset.hpp>
#include <cubos/engine/scene/scene.hpp>
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

        /// @brief Height at which the tile blocks projectiles and players.
        float blockHeight;
    };

    /// @brief Component which keeps information about the tile-map.
    ///
    /// On addition, immediately creates entities for the tile chunks.
    struct TileMap
    {
        CUBOS_REFLECT;

        /// @brief Map of tile indices to their voxel grids.
        std::vector<cubos::engine::Asset<cubos::engine::Scene>> types;

        /// @brief Tile-map data.
        std::vector<std::vector<Tile>> tiles;

        /// @brief Entity occupying each tile (if any).
        std::vector<std::vector<cubos::engine::Entity>> entities;
    };

    void tileMapPlugin(cubos::engine::Cubos& cubos);
} // namespace demo
