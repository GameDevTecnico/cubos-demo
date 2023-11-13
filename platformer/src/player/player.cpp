#include "player.hpp"

#include <cubos/core/ecs/component/reflection.hpp>

#include <cubos/core/reflection/external/primitives.hpp>

CUBOS_REFLECT_IMPL(demo::Player)
{
    return cubos::core::ecs::ComponentTypeBuilder<Player>("demo::Player")
        .withField("id", &Player::id)
        .withField("isOnGround", &Player::isOnGround)
        .build();
}
