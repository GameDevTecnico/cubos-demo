#pragma once

#include <glm/glm.hpp>

#include <cubos/core/reflection/reflect.hpp>

struct [[cubos::component("player", VecStorage)]] Player
{
    CUBOS_REFLECT;

    int id = 0;
    bool isOnGround = true;
};
