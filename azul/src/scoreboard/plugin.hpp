#pragma once

#include <cubos/engine/prelude.hpp>

#include <map>

namespace demo
{
    struct Scoreboard
    {
        CUBOS_REFLECT;

        /// @brief Mapping between player ID and score.
        std::map<int, int> scores;

        void increaseScore(int playerID);        
    };

    void scoreboardPlugin(cubos::engine::Cubos& cubos);
} // namespace demo