#pragma once

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/assets/asset.hpp>
#include <cubos/engine/scene/scene.hpp>
#include <cubos/engine/prelude.hpp>

#include <unordered_map>
#include <vector>
#include <string>

namespace demo
{
    /// @brief Component which creates child entities for each tile in a tile map.
    struct TileMap
    {
        CUBOS_REFLECT;

        std::unordered_map<std::string, cubos::engine::Asset<cubos::engine::Scene>> types;
        std::vector<std::vector<std::string>> tiles;
    };

    void tilesPlugin(cubos::engine::Cubos& cubos);
} // namespace demo
