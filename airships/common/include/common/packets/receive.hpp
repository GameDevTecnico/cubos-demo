#pragma once

#include <common/packets/pipe.hpp>
#include <common/packets/packet.hpp>
#include <cubos/core/reflection/reflect.hpp>

namespace airships::common
{
    struct PacketReceivePipe : PacketPipe<PacketData>
    {
        CUBOS_ANONYMOUS_REFLECT(PacketReceivePipe);
    };
} // namespace airships::common
