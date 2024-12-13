#pragma once

#include <cubos/engine/prelude.hpp>

namespace airships::client
{
    struct PlayerId
    {
        CUBOS_REFLECT;

        int id;
    };

    void playerIdPlugin(cubos::engine::Cubos& cubos);
} // namespace airships::client
