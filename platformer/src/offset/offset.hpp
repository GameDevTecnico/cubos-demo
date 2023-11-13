#pragma once

#include <cubos/core/reflection/reflect.hpp>
#include <cubos/core/ecs/entity/entity.hpp>

#include <glm/vec3.hpp>

namespace demo
{
    struct [[cubos::component("")]] Offset
    {
        CUBOS_REFLECT;

        glm::vec3 vec;
        cubos::core::ecs::Entity parent;
    };
} // namespace demo
