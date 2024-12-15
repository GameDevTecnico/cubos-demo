#pragma once

#include <cubos/engine/prelude.hpp>

namespace airships::client
{
    /// @brief Component which makes a drivable entity fall when it no longer has health.
    struct DrivableDestruction
    {
        CUBOS_REFLECT;

        /// @brief Height at which the entity is destroyed.
        float destroyHeight = 1.0F;
    };

    void drivableDestructionPlugin(cubos::engine::Cubos& cubos);
} // namespace airships::client
