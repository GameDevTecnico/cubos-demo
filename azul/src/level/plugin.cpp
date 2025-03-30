#include "plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>

using namespace cubos::engine;
 
CUBOS_REFLECT_IMPL(demo::Level)
{
    return cubos::core::ecs::TypeBuilder<Level>("demo::Level")
        .build();
}

void demo::levelPlugin(Cubos& cubos)
{
    cubos.component<Level>();
}