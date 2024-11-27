#include <common/packets/plugin.hpp>
#include <common/packets/receive.hpp>
#include <common/packets/send.hpp>
#include <common/packets/ctos/join/plugin.hpp>
#include <common/packets/stoc/disconnect/plugin.hpp>

void airships::common::packetsPlugin(cubos::engine::Cubos& cubos)
{
    cubos.resource<PacketSendPipe>();
    cubos.resource<PacketReceivePipe>();

    cubos.plugin(joinPacketPlugin);
    cubos.plugin(serverDisconnectPacketPlugin);
}
