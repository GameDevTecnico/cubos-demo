#include "plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(demo::Interaction)
{
    return cubos::core::ecs::TypeBuilder<Interaction>("demo::Interaction").build();
}

void demo::interactionPlugin(Cubos& cubos)
{
    cubos.component<Interaction>();
}
