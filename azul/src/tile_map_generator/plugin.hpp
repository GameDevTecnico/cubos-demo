#pragma once

#include "../waves_animator/plugin.hpp"
#include "../waves/plugin.hpp"

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

        /// @name Voxel grids for the different tile types.
        /// @{
        cubos::engine::Asset<cubos::engine::Scene> grass;
        cubos::engine::Asset<cubos::engine::Scene> mountain;
        cubos::engine::Asset<cubos::engine::Scene> sand;
        /// @}

        Waves waves;
        WavesAnimator wavesAnimator;
    };

    void tileMapGeneratorPlugin(cubos::engine::Cubos& cubos);
} // namespace demo
