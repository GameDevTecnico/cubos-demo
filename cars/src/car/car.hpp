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

        float maxAcceleration = 100.0F;
        float maxBreaking = 100.0F;
        float maxTurn = 0.04F;
        float forwardDragMul = 0.001F;
        float angularDragMul = 1.0F;
        float sideDragMul = 0.1F;
        float maxAngularVelocity = 10.0F;
        float roadGrip = 0.01F;
        float turnPeak = 50.0F;
        float turnDecay = 0.02F;
    };
} // namespace demo
