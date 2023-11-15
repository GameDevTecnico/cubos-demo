#pragma once

#include <cubos/core/reflection/reflect.hpp>

#include <glm/vec3.hpp>

namespace demo
{
    struct [[cubos::component("")]] Mover
    {
        CUBOS_REFLECT;

        glm::vec3 from;
        glm::vec3 to;
        float speed = 10.0F;
        bool direction = false;
        bool hasStarted = false;
    };
} // namespace demo
