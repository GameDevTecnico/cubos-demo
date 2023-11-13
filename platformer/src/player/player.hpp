#pragma once

#include <cubos/core/reflection/reflect.hpp>
#include <cubos/core/ecs/entity/entity.hpp>

namespace demo
{
    struct [[cubos::component("")]] Player
    {
        CUBOS_REFLECT;

        int id = 0;
        bool isOnGround = true;
    };
} // namespace demo
