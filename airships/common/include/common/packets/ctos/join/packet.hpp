#pragma once

#include <cubos/core/reflection/reflect.hpp>
#include <string>
#include <cubos/core/net/tcp_stream.hpp>
#include <memory>
#include <cstdint>

namespace airships::common
{
    struct JoinPacket
    {
        CUBOS_ANONYMOUS_REFLECT(JoinPacket);

        static constexpr uint16_t PacketId = 1;

        std::string name;

        std::shared_ptr<cubos::core::net::TcpStream> stream;
    };
} // namespace airships::common
