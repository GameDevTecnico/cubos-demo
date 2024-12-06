#pragma once

#include <cubos/core/reflection/reflect.hpp>
#include <cubos/engine/prelude.hpp>
#include <cubos/engine/api.hpp>

namespace airships::client
{
    struct BalloonInfo
    {
        CUBOS_REFLECT;

        enum class State
        {
            Empty,
            Holding,
            Popped
        };

        State state = State::Holding;
    };

    struct PopBalloon
    {
        CUBOS_REFLECT;
    };

    /// @brief plugin to handle balloon spawning
    /// @param cubos
    void balloonsPlugin(cubos::engine::Cubos& cubos);
} // namespace airships::client

CUBOS_REFLECT_EXTERNAL_DECL(CUBOS_EMPTY, airships::client::BalloonInfo::State);
