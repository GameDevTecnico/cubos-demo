#include "plugin.hpp"
#include "player.hpp"
#include "../game_server/plugin.hpp"
#include <common/packets/plugin.hpp>
#include <common/packets/ctos/join/plugin.hpp>

void airships::server::playerPlugin(cubos::engine::Cubos& cubos)
{
    cubos.depends(gameServerPlugin);
    cubos.depends(common::packetsPlugin);

    cubos.component<Player>();
}
