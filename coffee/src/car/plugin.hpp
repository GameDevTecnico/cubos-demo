#pragma once

#include <cubos/core/reflection/reflect.hpp>
#include <glm/vec3.hpp>
#include <cubos/engine/prelude.hpp>

namespace demo
{
    /// @brief Component which represents a car drivable by a player
    struct Car
    {
        CUBOS_REFLECT;

        int playerOwner = 0;

        int drivetrain = 1; // 0 is front wheel drive, 1 is rear wheel drive

        float accelInput = 0.0F;

        // engine
        float enginePower = 100.0F * 20.0F;
        float topSpeed = 100.0F;
        float minimumSpeed = -20.0F;
        float resistanceMassRatio = 10.0F; // lower will make the car slow down

        // suspension
        float suspensionRestDist = 5.5F;
        float wheelRadius = 1.25F;
        float springStrength = 30.0F;
        float springDamper = 60.0F;
    };

    /// @brief Component which represents a wheel/suspension of a car
    struct Wheel
    {
        CUBOS_REFLECT;

        int axis; // 0 is front, 1 is back;
        float gripFactor;
        float mass;

        // not implemented yet but we might want brakes/handbrake
        float brakeForce = 100.0F * 10.0F;
    };

    void carPlugin(cubos::engine::Cubos& cubos);
} // namespace demo
