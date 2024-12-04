#pragma once

#include <cubos/engine/prelude.hpp>

#include <string>

namespace airships::client
{
    /// @brief Component which makes an entity the steering wheel of its parent drivable entity.
    ///
    /// Assumes that the entity has a parent entity with a @ref Drivable component.
    /// Assumes that the entity has a child entity with the @ref SteeringWheelHead component.
    struct SteeringWheel
    {
        CUBOS_REFLECT;

        /// @brief Index of the player controlling this entity. Set to -1 if no player is controlling.
        int player = -1;

        /// @brief Name of the axis used to turn the wheel.
        std::string turnAxis = "horizontal";

        /// @brief Absolute value of the maximum turn angle of the wheel, in degrees.
        float maxTurnAngle = 180.0F;

        /// @brief Current turn angle of the wheel, in degrees.
        float turnAngle = 0.0F;

        /// @brief How much the wheel can turn per second, in degrees.
        float turnSpeed = 45.0F;

        /// @brief Target turn angle of the wheel, in degrees.
        float targetTurnAngle = 0.0F;

        /// @brief Factor which determines how fast the wheel lerps towards the target turn angle.
        float targetLerpFactor = 0.1F;
    };

    /// @brief Component which identifies the entity which represents the rotating part of a steering wheel.
    ///
    /// Just for visual purposes.
    struct SteeringWheelHead
    {
        CUBOS_REFLECT;
    };

    void steeringWheelPlugin(cubos::engine::Cubos& cubos);
} // namespace airships::client
