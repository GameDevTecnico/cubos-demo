#pragma once

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/prelude.hpp>
#include <cubos/engine/assets/asset.hpp>
#include <cubos/engine/scene/scene.hpp>

namespace demo
{
    /// @brief Component for skip machine entities.
    struct Skip
    {
        CUBOS_REFLECT;

        /// @brief Reward scene.
        cubos::engine::Asset<cubos::engine::Scene> reward;
    };

    void skipPlugin(cubos::engine::Cubos& cubos);
} // namespace demo
