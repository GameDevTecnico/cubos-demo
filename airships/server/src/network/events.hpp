#pragma once

#include <cubos/engine/prelude.hpp>
#include <cstdint>
#include <vector>

namespace airships::server
{
    struct PacketReceiveEvent
    {
        CUBOS_ANONYMOUS_REFLECT(PacketReceiveEvent);

        uint16_t id;
        std::vector<uint8_t> buf;
        cubos::engine::Entity entity;
    };

    struct PacketSendEvent
    {
        CUBOS_ANONYMOUS_REFLECT(PacketSendEvent);

        uint16_t id;
        std::vector<uint8_t> buf;
        cubos::engine::Entity entity;
    };
} // namespace airships::server
