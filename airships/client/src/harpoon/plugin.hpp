#pragma once

#include <cubos/engine/prelude.hpp>
#include <cubos/engine/assets/asset.hpp>
#include <cubos/engine/scene/scene.hpp>

namespace airships::client
{

    struct Harpoon
    {
        CUBOS_REFLECT;

        int player = -1;

        bool harpoonLoaded = false;

        cubos::engine::Asset<cubos::engine::Scene> bulletScene;
        float bulletSpeed = 100.0F;
    };

    struct HarpoonTube
    {
        CUBOS_REFLECT;
    };

    void harpoonPlugin(cubos::engine::Cubos& cubos);
} // namespace airships::client
