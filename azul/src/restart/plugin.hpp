#pragma once

#include <string>

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/assets/asset.hpp>
#include <cubos/engine/scene/scene.hpp>

#include <cubos/engine/prelude.hpp>

namespace demo
{
    struct Restart
    {
        CUBOS_REFLECT;

        cubos::engine::Asset<cubos::engine::Scene> scene;
        float accumT = 0;
    };

    void restartPlugin(cubos::engine::Cubos& cubos);
} // namespace demo
