#pragma once

#include <common/packets/pipe.hpp>
#include <common/packets/stoc/disconnect/packet.hpp>

namespace airships::common
{
    struct ServerDisconnectPacketPipe : PacketPipe<ServerDisconnectPacket>
    {
        CUBOS_ANONYMOUS_REFLECT(ServerDisconnectPacketPipe);
    };
} // namespace airships::common
