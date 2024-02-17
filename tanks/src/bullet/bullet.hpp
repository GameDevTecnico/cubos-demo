#pragma once

#include <cubos/core/reflection/reflect.hpp>
#include <glm/vec3.hpp>

struct Bullet
{
    CUBOS_REFLECT;

    int damage = 20;
    float time = 0.0F;
};
