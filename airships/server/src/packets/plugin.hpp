#pragma once

#include <cubos/engine/prelude.hpp>

namespace airships::server
{
    extern cubos::engine::Tag packetReceiverTag;
    extern cubos::engine::Tag inPacketHandlerTag;
    extern cubos::engine::Tag outPacketHandlerTag;
    extern cubos::engine::Tag packetSenderTag;

    void packetsPlugin(cubos::engine::Cubos& cubos);
} // namespace airships::server
