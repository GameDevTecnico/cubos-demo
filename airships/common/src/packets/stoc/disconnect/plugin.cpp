#include <common/packets/stoc/disconnect/plugin.hpp>
#include <common/packets/stoc/disconnect/pipe.hpp>

void airships::common::serverDisconnectPacketPlugin(cubos::engine::Cubos& cubos)
{
    cubos.resource<ServerDisconnectPacketPipe>();
}
