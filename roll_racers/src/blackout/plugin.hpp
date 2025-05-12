#pragma once

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/prelude.hpp>

namespace roll_racers
{
    /// @brief Component which identifies entities which should be destroyed when the blackout starts.
    struct BlackoutDestroy
    {
        CUBOS_REFLECT;
    };

    struct Semaphore
    {
        CUBOS_REFLECT;

        int stage{0};
        float intensity{0.0F};
    };

    void blackoutPlugin(cubos::engine::Cubos& cubos);
} // namespace roll_racers
