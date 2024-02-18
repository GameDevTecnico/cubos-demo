#pragma once

#include <cubos/core/reflection/reflect.hpp>
#include <glm/vec3.hpp>

struct Particle
{
    CUBOS_REFLECT;

    float startLife = 1.0F;
    float life = 1.0F;
    float size = 1.0F;
    glm::vec3 velocity;
};
