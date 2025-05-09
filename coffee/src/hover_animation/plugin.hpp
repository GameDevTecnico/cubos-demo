#pragma once

#include <cubos/engine/prelude.hpp>

namespace coffee
{
    struct HoverAnimation
    {
        CUBOS_REFLECT;

        float rotationPeriod{1.0F};
        float translationDistance{1.0F};
        float translationPeriod{2.0F};
        float timeAcc{0.0F};
    };

    void hoverAnimationPlugin(cubos::engine::Cubos& cubos);
} // namespace coffee
