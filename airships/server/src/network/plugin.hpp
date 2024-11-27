#pragma once

#include <cubos/engine/prelude.hpp>

namespace airships::server
{
    extern cubos::engine::Tag networkReceiveTag;

    extern cubos::engine::Tag networkSendTag;

    void networkPlugin(cubos::engine::Cubos& cubos);
} // namespace airships::server
