#pragma once

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/prelude.hpp>

namespace coffee
{
    /// @brief Component which makes a UI element blink
    struct UIBlink
    {
        CUBOS_REFLECT;
    };

    void uiEffectsPlugin(cubos::engine::Cubos& cubos);
} // namespace coffee
