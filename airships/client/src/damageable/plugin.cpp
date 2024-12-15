#include "plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(airships::client::Damageable)
{
    return cubos::core::ecs::TypeBuilder<Damageable>("airships::client::Damageable")
        .withField("health", &Damageable::health)
        .build();
}

void airships::client::damageablePlugin(Cubos& cubos)
{
    cubos.component<Damageable>();
}
