#include "player.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/string.hpp>

using namespace airships::server;

CUBOS_REFLECT_IMPL(Player)
{
    return cubos::core::ecs::TypeBuilder<Player>("airships::server::Player").withField("name", &Player::name).build();
}
