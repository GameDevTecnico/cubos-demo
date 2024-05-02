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
        float dayDuration = 120.0F;

        /// @brief How many days the players have survived.
        int daysSurvived = 0;

        /// @brief How much time has passed since the start of the day.
        ///
        /// If this value is greater than or equal to @ref dayDuration, then it's night.
        float timeOfDay = 0.0F;

        /// @brief Score needed to finish the night.
        int scoreToFinishNight{-1};

        /// @brief Current night score, resets every night.
        int score = 0;
    };

    void progressionPlugin(cubos::engine::Cubos& cubos);
} // namespace demo
