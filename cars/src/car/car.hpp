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
        float wheelVelocity = 0.0F;
        float wheelAngle = 0.0F; // In degrees.

        float wheelBase = 3.2F; // Distance between the front and rear wheels.
        float acceleration = 15.0F;
        float braking = 10.0F;
        float maxWheelVelocity = 20.0F;
        float steeringSpeed = 25.0F;
        float maxWheelAngle = 45.0F;

        // Maximum velocity transferred from the wheels to the car's velocity.
        float wheelGrip = 50.0F;

        // Constant drag force applied to the car's velocity. Multiplied by the wheel grip.
        float forwardDrag = 0.01F;
        float lateralDrag = 0.1F;

        // Drag force applied to the car's velocity multiplied by the car's velocity. Multiplied by the wheel grip.
        float forwardDragCoefficient = 0.001F;
        float lateralDragCoefficient = 0.01F;
    };
} // namespace demo
