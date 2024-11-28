#include <common/packets/stoc/disconnect/plugin.hpp>
#include <common/packets/stoc/disconnect/packet.hpp>

void airships::common::serverDisconnectPacketPlugin(cubos::engine::Cubos& cubos)
{
    cubos.event<ServerDisconnectPacket>();
}
