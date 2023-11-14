#pragma once

#include <glm/glm.hpp>

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
        float animationSpeed = 1.0F;
        float jumpForce = 1000.0F;
        float speed = 10.0F;
        cubos::core::ecs::Entity torso;
        cubos::core::ecs::Entity leftHand;
        cubos::core::ecs::Entity rightHand;
        cubos::core::ecs::Entity leftFoot;
        cubos::core::ecs::Entity rightFoot;
        glm::vec3 forward = {0.0F, 0.0F, 1.0F};
        glm::vec3 right = {1.0F, 0.0F, 0.0F};
    };
} // namespace demo
