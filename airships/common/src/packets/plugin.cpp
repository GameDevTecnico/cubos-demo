#include <common/packets/plugin.hpp>
#include <common/packets/receive.hpp>
#include <common/packets/send.hpp>

void airships::common::packetsPlugin(cubos::engine::Cubos& cubos)
{
    cubos.resource<PacketSendPipe>();
    cubos.resource<PacketReceivePipe>();
}
