#pragma once

#include <cubos/core/reflection/reflect.hpp>
#include <cubos/core/net/address.hpp>
#include <cubos/core/net/tcp_stream.hpp>
#include <vector>
#include <memory>
#include <cstdint>

namespace airships::common
{
    struct PacketReceiveEvent
    {
        CUBOS_ANONYMOUS_REFLECT(PacketReceiveEvent);

        uint16_t id;
        std::vector<uint8_t> buffer;
        std::shared_ptr<cubos::core::net::TcpStream> source;
    };

    struct PacketSendEvent
    {
        CUBOS_ANONYMOUS_REFLECT(PacketSendEvent);

        uint16_t id;
        std::vector<uint8_t> buffer;
        std::shared_ptr<cubos::core::net::TcpStream> dest;
    };
} // namespace airships::common
