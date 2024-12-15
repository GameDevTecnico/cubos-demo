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

        /// @brief Roll of the entity when turning at the maximum angular velocity, in degrees.
        float maxRoll = 0.0F;

        /// @brief Current roll of the entity, in degrees.
        float roll = 0.0F;

        /// @brief Interpolation factor for the roll.
        float rollLerpFactor = 0.1F;

        /// @brief Current yaw of the entity, in degrees.
        float yaw = 0.0F;

        /// @brief Current buoyancy of the entity.
        float buoyancy = 1.0F;
    };

    void drivablePlugin(cubos::engine::Cubos& cubos);
} // namespace airships::client
