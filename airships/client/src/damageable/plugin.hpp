#pragma once

#include <cubos/engine/prelude.hpp>

#include <string>

namespace airships::client
{
    /// @brief Component which allows an entity to be damaged.
    struct Damageable
    {
        CUBOS_REFLECT;

        /// @brief Amount of health the entity has.
        int health = 3;
    };

    void damageablePlugin(cubos::engine::Cubos& cubos);
} // namespace airships::client
