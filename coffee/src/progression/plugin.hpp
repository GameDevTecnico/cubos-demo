#pragma once

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/prelude.hpp>

namespace demo
{
    /// @brief Resource which holds the current progression of the game.
    struct Progression
    {
        CUBOS_REFLECT;

        /// @brief How long each day lasts in seconds.
        float dayDuration = 30.0F;

        /// @brief How many days have passed.
        int daysPassed = 0;

        /// @brief How much time has passed since the start of the day.
        float timeOfDay = 0.0F;
    };

    void progressionPlugin(cubos::engine::Cubos& cubos);
} // namespace demo
