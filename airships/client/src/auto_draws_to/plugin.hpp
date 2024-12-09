#pragma once

#include <cubos/engine/prelude.hpp>

namespace airships::client
{
    /// @brief Component which automatically adds a DrawsTo relation to any render target.
    struct AutoDrawsTo
    {
        CUBOS_REFLECT;
    };

    void autoDrawsToPlugin(cubos::engine::Cubos& cubos);
} // namespace airships::client