#pragma once

#include <cubos/engine/prelude.hpp>

#include <string>

namespace airships::client
{
    /// @brief Component which makes an entity rotate according to the target angular velocity of its parent drivable
    /// entity.
    ///
    /// Assumes that the entity has a parent entity has a relation @ref InterpolationOf with an entity with a @ref
    /// Drivable component.
    struct Rudder
    {
        CUBOS_REFLECT;

        /// @brief Absolute value of the maximum yaw of the rudder, in degrees.
        float maxYaw = 0.0F;

        /// @brief Absolute value of the maximum pitch of the rudder, in degrees.
        float maxPitch = 0.0F;

        /// @brief Value added to the yaw of the rudder, in degrees.
        float offsetYaw = 0.0F;

        /// @brief Value added to the pitch of the rudder, in degrees.
        float offsetPitch = 0.0F;
    };

    void rudderPlugin(cubos::engine::Cubos& cubos);
} // namespace airships::client
