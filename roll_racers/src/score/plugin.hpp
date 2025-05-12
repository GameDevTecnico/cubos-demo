#pragma once

#include <cubos/engine/prelude.hpp>

namespace roll_racers
{
    /// @brief Resource which holds the score of each player.
    struct PlayerScores
    {
        CUBOS_REFLECT;

        int scores[4] = {-1, -1, -1, -1};
        float scoreTimer = 0.0F;
    };

    void scorePlugin(cubos::engine::Cubos& cubos);
} // namespace roll_racers
