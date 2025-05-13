#include "plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(airships::client::Damageable)
{
    return cubos::core::ecs::TypeBuilder<Damageable>("airships::client::Damageable").wrap(&Damageable::health);
}

void airships::client::damageablePlugin(Cubos& cubos)
{
    cubos.component<Damageable>();
}
