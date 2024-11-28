#include <common/packets/ctos/join/plugin.hpp>
#include <common/packets/ctos/join/packet.hpp>

void airships::common::joinPacketPlugin(cubos::engine::Cubos& cubos)
{
    cubos.event<JoinPacket>();
}
