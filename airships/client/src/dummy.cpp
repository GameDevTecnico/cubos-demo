#include <cubos/core/net/tcp_stream.hpp>
#include <cubos/core/net/address.hpp>
#include <vector>
#include <common/packets/ctos/join/packet.hpp>

int main()
{
    cubos::core::net::TcpStream stream{};
    stream.connect(cubos::core::net::Address::Any, 26001);
    uint16_t packetId = airships::common::JoinPacket::PacketId;
    stream.write(&packetId, sizeof(packetId));
    std::vector<uint8_t> buf = {1, 2, 3, 4, 5};
    uint16_t size = static_cast<uint16_t>(buf.size());
    stream.write(&size, sizeof(size));
    stream.write(buf.data(), size);

    while (true)
    {
    }
    return 0;
}
