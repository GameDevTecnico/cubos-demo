#pragma once

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/prelude.hpp>

namespace demo
{
    /// @brief Component used to identify entities which can be held by other entities.
    struct Holdable
    {
        CUBOS_REFLECT;
    };

    void holdablePlugin(cubos::engine::Cubos& cubos);
} // namespace demo
