#include "plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(coffee::BlackoutDestroy)
{
    return cubos::core::ecs::TypeBuilder<BlackoutDestroy>("coffee::BlackoutDestroy").build();
}

void coffee::blackoutPlugin(Cubos& cubos)
{
    cubos.component<BlackoutDestroy>();
}
