#pragma once

#include <cubos/engine/prelude.hpp>
#include <cubos/engine/assets/asset.hpp>
#include <cubos/engine/scene/scene.hpp>

#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/vector.hpp>

#include <glm/glm.hpp>

namespace coffee
{
    struct MapConnector
    {
        CUBOS_REFLECT;

        glm::vec3 position;
        int rotation{0};
    };

    struct MapTile
    {
        CUBOS_REFLECT;

        cubos::engine::Asset<cubos::engine::Scene> scene;
        std::vector<MapConnector> connectors;
    };

    struct MapGenerator
    {
        CUBOS_REFLECT;

        std::size_t length{5};
        MapTile start;
        std::vector<MapTile> middle;
        MapTile end;
    };

    void mapGeneratorPlugin(cubos::engine::Cubos& cubos);
} // namespace coffee
