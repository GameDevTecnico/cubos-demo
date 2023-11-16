#pragma once

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/scene/scene.hpp>

namespace demo
{
    struct [[cubos::component("")]] Victory
    {
        CUBOS_REFLECT;

        int nextScene{};
    };
} // namespace demo
