#pragma once

#include <cubos/engine/prelude.hpp>

namespace airships::server
{
    extern cubos::engine::Tag incomingPacketHandlerTag;

    extern cubos::engine::Tag outgoingPacketHandlerTag;

    void packetHandlersPlugin(cubos::engine::Cubos& cubos);
} // namespace airships::server
