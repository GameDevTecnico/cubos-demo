#pragma once

#include <cubos/core/reflection/reflect.hpp>
#include <string>
#include <cubos/core/net/tcp_stream.hpp>
#include <memory>
#include <cstdint>

namespace airships::common
{
    struct ServerDisconnectPacket
    {
        CUBOS_REFLECT;

        static constexpr uint16_t PacketId = 2;

        std::string reason;

        std::shared_ptr<cubos::core::net::TcpStream> stream;
    };
} // namespace airships::common
