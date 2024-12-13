#pragma once

#include <cubos/engine/prelude.hpp>

#include <cubos/engine/scene/scene.hpp>
#include <cubos/engine/assets/asset.hpp>

#include <vector>

namespace airships::client
{
    struct TeamSpawner
    {
        CUBOS_REFLECT;

        struct Player
        {
            CUBOS_REFLECT;

            cubos::engine::Asset<cubos::engine::Scene> scene;
            int id;
        };

        cubos::engine::Asset<cubos::engine::Scene> boat;
        std::vector<Player> players;
    };

    void teamSpawnerPlugin(cubos::engine::Cubos& cubos);
} // namespace airships::client
