#pragma once

#include <cubos/engine/prelude.hpp>

#include <string>

namespace airships::client
{
    /// @brief Component which makes an entity rotate according to the target angular velocity of its parent drivable entity.
    ///
    /// Assumes that the entity has a parent entity with a @ref Drivable component.
    struct Rudder
    {
        CUBOS_REFLECT;

        /// @brief Absolute value of the maximum angle of the rudder, in degrees.
        float maxAngle = 180.0F;

        /// @brief Value added to the angle of the rudder, in degrees.
        float offsetAngle = 0.0F;
    };

    void rudderPlugin(cubos::engine::Cubos& cubos);
} // namespace airships::client
