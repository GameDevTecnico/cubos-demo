#pragma once

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/prelude.hpp>

namespace demo
{
    struct Level
    {
        CUBOS_REFLECT;
    };

    void levelPlugin(cubos::engine::Cubos& cubos);
} // namespace demo
