#pragma once

#include <cubos/core/reflection/reflect.hpp>
#include <cubos/engine/scene/scene.hpp>

#include <glm/vec3.hpp>

namespace demo
{
    struct [[cubos::component("")]] Cannon
    {
        CUBOS_REFLECT;

        cubos::engine::Asset<cubos::engine::Scene> bullet;
        float bulletTime = 10.0F;
        float timer = 0.0F;
    };
} // namespace demo
