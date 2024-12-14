#include "plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/transform/plugin.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(airships::client::Holdable)
{
    return cubos::core::ecs::TypeBuilder<Holdable>("airships::client::Holdable")
        .withField("airships::client::type", &Holdable::type)
        .build();
}

void airships::client::holdablePlugin(Cubos& cubos)
{
    cubos.depends(transformPlugin);

    cubos.component<Holdable>();
}
