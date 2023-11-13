#pragma once

#include <cubos/core/reflection/reflect.hpp>

namespace demo
{
    struct [[cubos::component("player")]] Player
    {
        CUBOS_REFLECT;

        int id = 0;
        bool isOnGround = true;
    };
} // namespace demo
