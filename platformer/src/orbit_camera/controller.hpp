#pragma once

#include <cubos/core/reflection/reflect.hpp>
#include <cubos/core/ecs/entity/entity.hpp>
#include <cubos/engine/input/bindings.hpp>

#include <glm/vec3.hpp>

namespace demo
{
    struct [[cubos::component("")]] OrbitCameraController
    {
        CUBOS_REFLECT;

        int playerId = 0;
        cubos::core::ecs::Entity target;
        glm::vec3 center = {0.0F, 0.0F, 0.0F};
        float speed = 1.0F;
        float rotationSpeed = 10.0F;
        float zoomSpeed = 10.0F;
        float distance = 100.0F;
        float theta = 0.0F;
        float phi = 45.0F;
    };
} // namespace demo
