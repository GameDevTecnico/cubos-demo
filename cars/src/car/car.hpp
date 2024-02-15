#pragma once

#include <cubos/core/reflection/reflect.hpp>
#include <glm/vec3.hpp>

namespace demo
{
    struct Car
    {
        CUBOS_REFLECT;

        int player;

        glm::vec3 linearVelocity = {0.0f, 0.0f, 0.0f};
        float angularVelocity = 0.0f;
        float wheelAngle = 0.0F; // In degrees.

        float wheelBase = 3.2F; // Distance between the front and rear wheels.
        float acceleration = 5.0F;
        float braking = 20.0F;
        float maxVelocity = 20.0F;
        float steeringSpeed = 30.0F;
        float maxWheelAngle = 15.0F;

        // How much centripetal force the car can handle before drifting.
        float wheelGrip = 0.3F;

        // Constant drag force applied to the car's velocity. Multiplied by the wheel grip.
        float forwardDrag = 0.1F;
        float lateralDrag = 0.0F;

        // Drag force applied to the car's velocity multiplied by the car's velocity. Multiplied by the wheel grip.
        float forwardDragCoefficient = 0.01F;
        float lateralDragCoefficient = 0.0F;
    };
} // namespace demo
