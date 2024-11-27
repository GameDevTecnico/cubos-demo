#pragma once

#include <cubos/core/reflection/reflect.hpp>
#include <memory>
#include <cubos/core/net/tcp_stream.hpp>

namespace airships::server
{
    struct Player
    {
        CUBOS_REFLECT;

        std::string name;
        std::shared_ptr<cubos::core::net::TcpStream> stream;
    };
} // namespace airships::server
