#include "plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(demo::Holdable)
{
    return cubos::core::ecs::TypeBuilder<Holdable>("demo::Holdable")
        .withField("stackable", &Holdable::stackable)
        .build();
}

void demo::holdablePlugin(Cubos& cubos)
{
    cubos.component<Holdable>();
}
