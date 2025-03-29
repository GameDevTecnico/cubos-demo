#pragma once

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/prelude.hpp>
#include <cubos/engine/scene/scene.hpp>
#include <cubos/engine/assets/asset.hpp>

#include <vector>

namespace demo
{
    /// @brief Component which animates waves.
    ///
    /// On addition, immediately creates entities for each tile.
    struct WavesAnimator
    {
        CUBOS_REFLECT;

        cubos::engine::Asset<cubos::engine::Scene> water;
        std::vector<std::vector<cubos::engine::Entity>> entities;
        float waterLerpFactor{0.5F};
    };

    void wavesAnimatorPlugin(cubos::engine::Cubos& cubos);
} // namespace demo
