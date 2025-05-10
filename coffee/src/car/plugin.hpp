#pragma once

#include <cubos/core/reflection/reflect.hpp>
#include <glm/vec3.hpp>
#include <cubos/engine/prelude.hpp>

namespace coffee
{
    struct PlayerOwner
    {
        CUBOS_REFLECT;

        int player = 0;
        bool canMove = true;
    };

    struct PlayerCameraOwner
    {
        CUBOS_REFLECT;

        int player = 0;
    };

    /// @brief Component which represents a car drivable by a player
    struct Car
    {
        CUBOS_REFLECT;

        int drivetrain = 1; // 0 is front wheel drive, 1 is rear wheel drive

        float accelInput = 0.0F;
        float steerInput = 0.0F;
        bool handBrakeOn = false;

        float headLightIntensity = 2.0F;     // Intensity of the head lights.
        float highBackLightIntensity = 3.0F; // Intensity of the back lights when reversing or braking.
        float lowBackLightIntensity = 1.0F;  // Minimum intensity of the back lights.
        float lightIntensityHalfTime = 0.5F; // Time it takes for the lights to reach half of the desired intensity.

        float dragConstant = 0.0F;              // Multiplied by the square of the velocity.
        float rollingResistanceConstant = 0.0F; // Multiplied by the velocity.

        // suspension
        float suspensionRestDist = 5.5F;
        float wheelRadius = 1.25F;
        float springStrength = 30.0F;
        float springDamper = 60.0F;

        float minSuspensionHeight = 0.0F; // Minimum suspension height, only used to animate the wheels
        float suspensionHalfTime =
            1.0F; // Time it takes for the tire model to reach half of the desired suspension height

        float fastWheelSteeringAngle = 0.0F;    // The maximum angle the wheel turns in degrees above peak velocity.
        float idleWheelSteeringAngle = 0.0F;    // The angle the wheel turns when the car is idle in degrees.
        float fastWheelSteeringVelocity = 0.0F; // The speed at which the wheel turns to the fastWheelSteeringAngle.
        float currentWheelSteeringAngle = 0.0F; // The angle the wheel is currently at in degrees.
        float wheelSteeringHalfTime = 1.0F;     // Time it takes for the tires to rotate halfway to the desired angle

        float tractionConstant = 0.0F; // Constant multiplied by the slip ratio to get the traction force.
        float tractionPeakSlip = 0.0F; // Slip ratio at which the peak traction occurs.

        float corneringConstant = 0.0F;      // Constant multiplied by the slip angle to get the cornering force.
        float corneringPeakSlipAngle = 0.0F; // Slip angle (degrees) at which the peak cornering occurs.

        float driveTorque = 0.0F; // Torque applied to the wheels when accelerating.
    };

    /// @brief Component which represents a wheel/suspension of a car
    struct Wheel
    {
        CUBOS_REFLECT;

        int axis; // 0 is front, 1 is back;
        float gripFactor;
        float angularVelocity = 0.0F;
        float mass;

        // not implemented yet but we might want brakes/handbrake
        float brakeForce = 100.0F * 10.0F;

        // The distance from the wheel to the ground, or the suspension rest distance.
        // Determined automatically by the car plugin.
        float currentSuspensionHeight;

        // Current velocity of the wheel.
        // Determined automatically by the car plugin.
        float currentVelocity;
    };

    /// @brief Component which identifies the horn.
    struct Horn
    {
        CUBOS_REFLECT;
    };

    void carPlugin(cubos::engine::Cubos& cubos);
} // namespace coffee
