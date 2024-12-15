#pragma once

#include <cubos/engine/prelude.hpp>
#include <cubos/core/reflection/reflect.hpp>

struct StormInfo
{
    CUBOS_REFLECT;

    // added this just to help with spawning islands and balloons
    int stormRadius = 500;
};

namespace airships::client
{
    void stormPlugin(cubos::engine::Cubos& cubos);
} // namespace airships::client
