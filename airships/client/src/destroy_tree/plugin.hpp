#pragma once

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/prelude.hpp>

namespace airships::client
{
    /// @brief When added, destroys the entity and all of its children and interpolations.
    struct DestroyTree
    {
        CUBOS_REFLECT;
    };

    void destroyTreePlugin(cubos::engine::Cubos& cubos);
} // namespace airships::client
