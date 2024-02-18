#pragma once

#include <cubos/core/reflection/reflect.hpp>

struct Tank{
    CUBOS_REFLECT;
    int player;
};

struct TankTower
{
    CUBOS_REFLECT;
    float rotationVelocity;
};

struct TankBody
{
    CUBOS_REFLECT;
    float speed;
    float angularVelocity;
};
