#pragma once

#include <cubos/engine/prelude.hpp>
#include <cubos/engine/assets/asset.hpp>
#include <cubos/engine/scene/scene.hpp>

#include <glm/glm.hpp>

namespace coffee
{
    struct MapGenerator
    {
        CUBOS_REFLECT;

        glm::ivec2 tileSize{};
        std::size_t trackLength{};
        cubos::engine::Asset<cubos::engine::Scene> startTileScene;
        cubos::engine::Asset<cubos::engine::Scene> endTileScene;
        cubos::engine::Asset<cubos::engine::Scene> straightTileScene;
        cubos::engine::Asset<cubos::engine::Scene> curveLeftTileScene;
        cubos::engine::Asset<cubos::engine::Scene> curveRightTileScene;
        cubos::engine::Asset<cubos::engine::Scene> orangeBuildingTileScene;
        cubos::engine::Asset<cubos::engine::Scene> blackBuildingTileScene;
        cubos::engine::Asset<cubos::engine::Scene> whiteBuildingTileScene;
    };

    void mapGeneratorPlugin(cubos::engine::Cubos& cubos);
} // namespace coffee
