#pragma once

#include <cubos/core/ecs/entity/entity.hpp>
#include <cubos/core/reflection/reflect.hpp>

struct Health
{
    CUBOS_REFLECT;

    int points;
};
