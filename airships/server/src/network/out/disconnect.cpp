#include "disconnect.hpp"
#include "../plugin.hpp"
#include "../../game_server/plugin.hpp"

using namespace cubos::engine;

void airships::server::disconnectPacketPlugin(Cubos& cubos)
{
    cubos.depends(networkPlugin);
    cubos.depends(gameServerPlugin);
}
