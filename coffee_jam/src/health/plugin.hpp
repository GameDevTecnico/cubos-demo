#pragma once

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/prelude.hpp>

namespace demo
{
    /// @brief Component used for anything that has health.
    struct Health
    {
        CUBOS_REFLECT;

        /// @brief Health points .
        int hp = 1;
        int team = 0;
    };

    void healthPlugin(cubos::engine::Cubos& cubos);
} // namespace demo