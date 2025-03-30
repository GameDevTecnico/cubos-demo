#pragma once

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/prelude.hpp>

namespace demo
{
    /// @brief Holds rotating animation data.
    struct RotatingAnimation
    {
        CUBOS_REFLECT;

        float baseHeight{0.0F};
        float time{0.0F};
        float speed{1.0F};
    };

    void rotatingAnimationPlugin(cubos::engine::Cubos& cubos);
} // namespace demo