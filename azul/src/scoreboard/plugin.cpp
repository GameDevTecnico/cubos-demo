#include "plugin.hpp"

#include <cubos/core/reflection/external/vector.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/ecs/reflection.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(demo::Scoreboard)
{
    return cubos::core::ecs::TypeBuilder<Scoreboard>("demo::Scoreboard")
        .build();
}

void demo::scoreboardPlugin(Cubos& cubos)
{
    cubos.resource<Scoreboard>();
}

void demo::Scoreboard::increaseScore(int playerID)
{
    if( scores.contains( playerID ) ) {
        scores[playerID] += 1;
    } else {
        scores[playerID] = 1;
    }
}