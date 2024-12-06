#include "game_server.hpp"

namespace airships::server
{
    GameServer::GameServer(uint16_t port) : mListener(cubos::core::net::TcpListener{})
    {
        mListener.listen(cubos::core::net::Address::Any, port);
        mListener.setBlocking(false);
    }

    const cubos::core::net::TcpListener& GameServer::listener() const
    {
        return mListener;
    }

    std::chrono::milliseconds GameServer::tickStart() const
    {
        return mTickStart;
    }

    void GameServer::startTick()
    {
        using namespace std::chrono;
        mTickStart = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
    }

} // namespace airships::server
