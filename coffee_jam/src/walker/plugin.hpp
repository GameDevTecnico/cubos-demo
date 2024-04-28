#pragma once

#include <glm/vec2.hpp>

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/prelude.hpp>

namespace demo
{
    /// @brief Component which can be used to move an entity in a grid-like fashion.
    struct Walker
    {
        CUBOS_REFLECT;

        /// @brief Where the entity is currently in (or moving from), in tile coordinates.
        glm::ivec2 position = {0, 0};

        /// @brief Direction in which the entity is moving. Automatically reset to {0, 0} when the entity arrives.
        glm::ivec2 direction = {0, 0};

        /// @brief Jump height of the entity.
        float jumpHeight = 0.0F;

        /// @brief Move speed of the entity.
        float moveSpeed = 1.0F;

        /// @brief Time which the entity takes to rotate half of the way to the target rotation.
        float halfRotationTime = 1.0F;

        /// @brief Animation progress of the current movement, from 0.0 to 1.0.
        float progress = 0.0F;
    };

    void walkerPlugin(cubos::engine::Cubos& cubos);
} // namespace demo
