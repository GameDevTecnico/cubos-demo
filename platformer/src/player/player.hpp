#pragma once

#include <cubos/core/reflection/reflect.hpp>
#include <cubos/core/ecs/entity/entity.hpp>

namespace demo
{
    struct [[cubos::component("")]] Player
    {
        CUBOS_REFLECT;

        int id = 0;
        bool isOnGround = false;
        float animationTime = 0.0F;
        cubos::core::ecs::Entity torso;
        cubos::core::ecs::Entity leftHand;
        cubos::core::ecs::Entity rightHand;
        cubos::core::ecs::Entity leftFoot;
        cubos::core::ecs::Entity rightFoot;
    };
} // namespace demo
