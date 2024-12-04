#pragma once

#include <cubos/engine/prelude.hpp>

#include <string>

namespace airships::client
{
    /// @brief Component which allows an entity to be driven by a player.
    struct Drivable
    {
        CUBOS_REFLECT;

        /// @brief Top linear velocity, in units per second.
        float topLinearVelocity = 1.0F;

        /// @brief Current linear velocity, in units per second.
        float linearVelocity = 0.0F;

        /// @brief Linear acceleration, in units per second per second.
        float linearAcceleration = 1.0F;

        /// @brief Maximum absolute turn speed at top linear velocity, in degrees per second.
        ///
        /// Decreases linearly with linear velocity.
        float topAngularVelocity = 1.0F;

        /// @brief Current turn speed, in degrees per second.
        float angularVelocity = 0.0F;

        /// @brief The desired angular velocity, in degrees per second.
        float targetAngularVelocity = 0.0F;

        /// @brief Turn acceleration, in degrees per second per second.
        float angularAcceleration = 1.0F;
    };

    void drivablePlugin(cubos::engine::Cubos& cubos);
} // namespace airships::client
