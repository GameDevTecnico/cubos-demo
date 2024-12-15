#pragma once

#include "../player/plugin.hpp"
#include "../boat_skin/plugin.hpp"

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

            PlayerSkin skin;
            int id;
        };

        cubos::engine::Asset<cubos::engine::Scene> boatScene;
        BoatSkin boatSkin;
        cubos::engine::Asset<cubos::engine::Scene> playerScene;
        std::vector<Player> players;
    };

    void teamSpawnerPlugin(cubos::engine::Cubos& cubos);
} // namespace airships::client
