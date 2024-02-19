#pragma once

#include <cubos/core/reflection/reflect.hpp>

struct Tank
{
    CUBOS_REFLECT;
    int player;
    float speed;
    float angularVelocity;
};

struct Turret
{
    CUBOS_REFLECT;
    int player;
    float rotationVelocity;
};
