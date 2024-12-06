#include "plugin.hpp"
#include "player.hpp"
#include <common/packets/ctos/join.hpp>

void airships::server::playerPlugin(cubos::engine::Cubos& cubos)
{
    cubos.component<Player>();
}
