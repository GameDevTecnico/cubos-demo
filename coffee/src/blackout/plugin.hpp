#pragma once

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/prelude.hpp>

namespace coffee
{
    /// @brief Component which identifies entities which should be destroyed when the blackout starts.
    struct BlackoutDestroy
    {
        CUBOS_REFLECT;
    };

    void blackoutPlugin(cubos::engine::Cubos& cubos);
} // namespace coffee
