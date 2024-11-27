#pragma once

#include <common/packets/pipe.hpp>
#include <common/packets/ctos/join/packet.hpp>

namespace airships::common
{
    struct JoinPacketPipe : PacketPipe<JoinPacket>
    {
        CUBOS_ANONYMOUS_REFLECT(JoinPacketPipe);
    };
} // namespace airships::common
