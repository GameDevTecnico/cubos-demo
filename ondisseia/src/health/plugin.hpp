#pragma once

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/prelude.hpp>

namespace demo
{
    enum Team
    {
        PLAYER_1,
        PLAYER_2,
        PLAYER_3,
        PLAYER_4
    };

    struct Damage
    {
        CUBOS_REFLECT;

        int hp = 1;
    };

    struct Health
    {
        CUBOS_REFLECT;

        int hp = 3;
        Team team = Team::PLAYER_1;
    };

    void healthPlugin(cubos::engine::Cubos& cubos);
} // namespace demo

CUBOS_REFLECT_EXTERNAL_DECL(CUBOS_EMPTY, demo::Team);