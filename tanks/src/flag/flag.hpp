#pragma once

#include <cubos/core/ecs/entity/entity.hpp>
#include <cubos/core/reflection/reflect.hpp>

struct Flag
{
    CUBOS_REFLECT;

    int range = 4;
    float progressPlayer1;
    float progressPlayer2;
    bool isClosePlayer1;
    bool isClosePlayer2;
};
