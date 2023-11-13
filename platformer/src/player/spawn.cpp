#include "spawn.hpp"

#include <cubos/core/ecs/component/reflection.hpp>

#include <cubos/core/reflection/external/primitives.hpp>

CUBOS_REFLECT_IMPL(demo::PlayerSpawn)
{
    return cubos::core::ecs::ComponentTypeBuilder<PlayerSpawn>("demo::PlayerSpawn")
        .withField("playerId", &PlayerSpawn::playerId)
        .withField("bindings", &PlayerSpawn::bindings)
        .build();
}
