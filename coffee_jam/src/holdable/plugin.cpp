#include "plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(demo::Holdable)
{
    return cubos::core::ecs::TypeBuilder<Holdable>("demo::Holdable").build();
}

void demo::holdablePlugin(Cubos& cubos)
{
    cubos.component<Holdable>();
}
