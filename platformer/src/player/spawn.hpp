#pragma once

#include <cubos/core/reflection/reflect.hpp>
#include <cubos/engine/input/bindings.hpp>
#include <cubos/engine/assets/asset.hpp>

namespace demo
{
    struct [[cubos::component("")]] PlayerSpawn
    {
        CUBOS_REFLECT;

        int playerId = 0;
        cubos::engine::Asset<cubos::engine::InputBindings> bindings;
    };
} // namespace demo
