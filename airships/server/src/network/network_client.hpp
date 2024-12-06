#pragma once

#include <cubos/core/net/tcp_stream.hpp>
#include <cubos/core/reflection/reflect.hpp>
#include <memory>

namespace airships::server
{
    struct NetworkClient
    {
        CUBOS_ANONYMOUS_REFLECT(NetworkClient);

        std::unique_ptr<cubos::core::net::TcpStream> stream;
        bool loggedIn{false};
    };
} // namespace airships::server
