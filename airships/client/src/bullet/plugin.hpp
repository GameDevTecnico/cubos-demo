#pragma once

#include <cubos/engine/prelude.hpp>

namespace airships::client
{
    /// @brief Component which makes an entity damage other entities on collision.
    ///
    /// Destroys the entity on collision.
    struct Bullet
    {
        CUBOS_REFLECT;
    };

    void bulletPlugin(cubos::engine::Cubos& cubos);
} // namespace airships::client
