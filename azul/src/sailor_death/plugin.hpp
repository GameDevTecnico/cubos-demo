#pragma once

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/prelude.hpp>

namespace demo
{
    /// @brief Relation linking sailor entities and their player entities.
    ///
    /// Animates the sailor's death.
    struct SailorDeath
    {
        CUBOS_REFLECT;

        /// @brief HP level at which the sailor dies. 
        int hp = 0;
    };

    void sailorDeathPlugin(cubos::engine::Cubos& cubos);
} // namespace demo