#pragma once

#include "../numerals/plugin.hpp"
#include "../rotating_animation/plugin.hpp"

#include <glm/vec2.hpp>
#include <unordered_map>

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/prelude.hpp>
#include <cubos/engine/assets/asset.hpp>
#include <cubos/engine/scene/scene.hpp>

namespace demo
{
    /// @brief Component which spawns a player and its score.
    struct ScoreIndicator
    {
        CUBOS_REFLECT;

        /// @brief Base numeral.
        Numeral numeral;

        /// @brief Base player rotation animation.
        RotatingAnimation rotatingAnimation;

        /// @brief Player model scenes.
        std::unordered_map<int, cubos::engine::Asset<cubos::engine::Scene>> playerScenes;
    };

    void scoreIndicatorPlugin(cubos::engine::Cubos& cubos);
} // namespace demo
