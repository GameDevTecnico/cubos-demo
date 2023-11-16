#pragma once

#include <cubos/core/reflection/reflect.hpp>

#include <glm/vec3.hpp>

namespace demo
{
    struct [[cubos::component("")]] Bullet
    {
        CUBOS_REFLECT;

        glm::vec3 velocity = {0.0F, 0.0F, 0.0F};
        float life = 5.0F;
    };
} // namespace demo
