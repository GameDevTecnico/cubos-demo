#pragma once

#include <cubos/engine/prelude.hpp>
#include <vector>
#include <cubos/core/net/tcp_listener.hpp>
#include <cubos/core/net/address.hpp>

namespace airships::server
{
    class GameServer
    {
    public:
        CUBOS_ANONYMOUS_REFLECT(GameServer);

        GameServer(uint16_t port);

        const cubos::core::net::TcpListener& listener() const;

        const std::vector<std::shared_ptr<cubos::core::net::TcpStream>>& clients() const;

        void connect(std::shared_ptr<cubos::core::net::TcpStream> client);

        std::chrono::milliseconds tickStart() const;

        void startTick();

    private:
        cubos::core::net::TcpListener mListener;
        std::vector<std::shared_ptr<cubos::core::net::TcpStream>> mConnectedClients;
        std::chrono::milliseconds mTickStart;
    };
} // namespace airships::server
