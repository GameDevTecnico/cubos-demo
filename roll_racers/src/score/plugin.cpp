#include "plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/glm.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(roll_racers::PlayerScores)
{
    return cubos::core::ecs::TypeBuilder<PlayerScores>("roll_racers::PlayerScores").build();
}

void roll_racers::scorePlugin(Cubos& cubos)
{
    cubos.resource<PlayerScores>();
}
