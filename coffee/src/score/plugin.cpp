#include "plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/glm.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(coffee::PlayerScores)
{
    return cubos::core::ecs::TypeBuilder<PlayerScores>("coffee::PlayerScores").build();
}

void coffee::scorePlugin(Cubos& cubos)
{
    cubos.resource<PlayerScores>();
}
