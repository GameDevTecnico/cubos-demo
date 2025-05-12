#pragma once

#include <cubos/engine/prelude.hpp>
#include <cubos/engine/scene/scene.hpp>
#include <cubos/engine/assets/asset.hpp>

namespace roll_racers
{
    struct SpawnOrphan
    {
        CUBOS_REFLECT;

        cubos::engine::Asset<cubos::engine::Scene> scene;
    };

    void spawnOrphanPlugin(cubos::engine::Cubos& cubos);
} // namespace roll_racers