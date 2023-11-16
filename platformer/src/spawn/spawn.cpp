#include "spawn.hpp"

#include <cubos/core/ecs/component/reflection.hpp>

#include <cubos/core/reflection/external/primitives.hpp>

CUBOS_REFLECT_IMPL(demo::Spawn)
{
    return cubos::core::ecs::ComponentTypeBuilder<Spawn>("demo::Spawn")
        .withField("playerId", &Spawn::playerId)
        .withField("bindings", &Spawn::bindings)
        .build();
}
