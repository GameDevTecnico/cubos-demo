#pragma once

#include <cubos/engine/prelude.hpp>
#include <cubos/engine/assets/asset.hpp>
#include <cubos/engine/scene/scene.hpp>

namespace airships::client
{

    struct Cannon
    {
        CUBOS_REFLECT;

        int player = -1;

        bool cannonLoaded = false;

        cubos::engine::Asset<cubos::engine::Scene> bulletScene;
        float bulletSpeed = 100.0F;
    };

    struct CannonTube
    {
        CUBOS_REFLECT;
    };

    void cannonPlugin(cubos::engine::Cubos& cubos);
} // namespace airships::client
