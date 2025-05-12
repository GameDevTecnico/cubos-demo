#pragma once

#include <cubos/engine/prelude.hpp>
#include <cubos/engine/assets/asset.hpp>
#include <cubos/engine/scene/scene.hpp>

#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/unordered_map.hpp>

#include <unordered_map>

#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

namespace roll_racers
{
    struct MapTile
    {
        CUBOS_REFLECT;

        cubos::engine::Asset<cubos::engine::Scene> scene;
        int rotation;
    };

    struct Map
    {
        CUBOS_REFLECT;

        std::unordered_map<glm::ivec2, MapTile> tiles;
        glm::ivec2 tileSize;
    };

    void mapPlugin(cubos::engine::Cubos& cubos);
} // namespace roll_racers
