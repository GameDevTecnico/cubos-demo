#pragma once

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/prelude.hpp>

namespace demo
{
    /// @brief Component for skip machine entities.
    struct Skip
    {
        CUBOS_REFLECT;
    };

    void skipPlugin(cubos::engine::Cubos& cubos);
} // namespace demo
