#pragma once

#include <cubos/engine/prelude.hpp>
#include <cubos/engine/render/lights/environment.hpp>
#include <cubos/engine/render/lights/directional.hpp>

namespace roll_racers
{
    struct DayNightManager
    {
        CUBOS_REFLECT;

        /// @brief Current state of the day-night cycle, where 0.0 is full day and 1.0 is full night.
        float state = 0.25F;

        /// @brief Desired state of the day-night cycle.
        float desiredState = 1.0F;

        /// @brief How fast the state changes.
        float speed = 0.75F;

        /// @brief Environment for the night.
        cubos::engine::RenderEnvironment nightEnvironment;

        /// @brief Sun light during the night.
        cubos::engine::DirectionalLight nightSunLight;

        /// @brief Environment for the day.
        cubos::engine::RenderEnvironment dayEnvironment;

        /// @brief Sun light during the day.
        cubos::engine::DirectionalLight daySunLight;
    };

    struct CarLight
    {
        CUBOS_REFLECT;
    };

    void dayNightPlugin(cubos::engine::Cubos& cubos);
} // namespace roll_racers
