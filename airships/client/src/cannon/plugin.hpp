#pragma once

#include <cubos/engine/prelude.hpp>

#include <glm/gtc/quaternion.hpp>

namespace airships::client
{

    struct Cannon
    {
        CUBOS_REFLECT;

        int player = -1;

        bool cannonLoaded = false;
    };

    struct CannonTube
    {
        CUBOS_REFLECT;
    };

    void cannonPlugin(cubos::engine::Cubos& cubos);
} // namespace airships::client
