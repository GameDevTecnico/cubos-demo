#include <common/packets/plugin.hpp>
#include <common/packets/packet.hpp>
#include <common/packets/ctos/join/plugin.hpp>
#include <common/packets/stoc/disconnect/plugin.hpp>

void airships::common::packetsPlugin(cubos::engine::Cubos& cubos)
{
    cubos.event<PacketSendEvent>();
    cubos.event<PacketReceiveEvent>();

    cubos.plugin(joinPacketPlugin);
    cubos.plugin(serverDisconnectPacketPlugin);
}
