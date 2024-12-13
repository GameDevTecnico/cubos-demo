#pragma once

#include <cubos/engine/prelude.hpp>

#include <cubos/engine/scene/scene.hpp>
#include <cubos/engine/assets/asset.hpp>

namespace airships::client
{
    struct LevelGenerator
    {
        CUBOS_REFLECT;

        struct Object
        {
            CUBOS_REFLECT;

            cubos::engine::Asset<cubos::engine::Scene> scene;
            int amount;
        };

        std::vector<Object> objects;
    };

    void levelGeneratorPlugin(cubos::engine::Cubos& cubos);
} // namespace airships::client
