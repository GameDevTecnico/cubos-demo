#pragma once

#include <glm/vec2.hpp>

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/prelude.hpp>
#include <cubos/engine/assets/asset.hpp>
#include <cubos/engine/scene/scene.hpp>

namespace demo
{
    /// @brief Relation linking sailor entities and their player entities.
    ///
    /// Animates the sailor's death.
    struct Numeral
    {
        CUBOS_REFLECT;

        /// @brief 0 model.
        cubos::engine::Asset<cubos::engine::Scene> model0;

        /// @brief I model.
        cubos::engine::Asset<cubos::engine::Scene> modelI;

        /// @brief V model.
        cubos::engine::Asset<cubos::engine::Scene> modelV;

        /// @brief X model.
        cubos::engine::Asset<cubos::engine::Scene> modelX;

        /// @brief Number to display.
        int number;
    };

    void numeralsPlugin(cubos::engine::Cubos& cubos);
} // namespace demo