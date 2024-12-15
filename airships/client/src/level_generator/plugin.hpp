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
            float minHeight{-100.0F};
            float maxHeight{100.0F};
        };

        std::vector<Object> objects;
    };

    void levelGeneratorPlugin(cubos::engine::Cubos& cubos);
} // namespace airships::client
