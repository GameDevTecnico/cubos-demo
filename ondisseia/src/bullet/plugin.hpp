#pragma once

#include <glm/vec2.hpp>

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/prelude.hpp>

namespace demo
{
    /// @brief Component for entities which represent the bullets.
    struct Bullet
    {
        CUBOS_REFLECT;

        /// @brief Origin of the bullet, in tile coordinates.
        glm::ivec2 origin = {0, 0};

        /// @brief Target of the bullet, in tile coordinates.
        glm::ivec2 target = {0, 0};

        /// @brief Base height of the bullet trajectory.
        float baseHeight = 0.0F;

        /// @brief Maximum height of the bullet trajectory.
        float maxHeight = 2.0F;

        /// @brief Animation progress of the bullet trajectory, from 0.0 to 1.0.
        float progress = 0.0F;

        /// @brief How much progress the bullet makes per second.
        float speed = 1.0F;
    };

    void bulletPlugin(cubos::engine::Cubos& cubos);
} // namespace demo
