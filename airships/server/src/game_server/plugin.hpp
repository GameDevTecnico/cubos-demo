#pragma once

#include <cubos/engine/prelude.hpp>

namespace airships::server
{
    extern cubos::engine::Tag serverTickStartTag;

    extern cubos::engine::Tag serverTickTag;

    extern cubos::engine::Tag serverTickEndTag;

    void gameServerPlugin(cubos::engine::Cubos& cubos);
} // namespace airships::server
