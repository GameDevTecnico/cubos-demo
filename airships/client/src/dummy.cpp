#include <cubos/core/net/tcp_stream.hpp>
#include <cubos/core/net/address.hpp>
#include <vector>
#include <common/packets/ctos/join/packet.hpp>
#include <iostream>

int main(int arhc, char** argv)
{
    cubos::core::net::TcpStream stream{};
    stream.connect(cubos::core::net::Address::Any, 26001);
    uint16_t packetId = airships::common::JoinPacket::PacketId;
    stream.write(&packetId, sizeof(packetId));
    std::vector<uint8_t> buf = {1, 2, 3, 4, 5};
    std::string name = argv[1];
    uint16_t size = static_cast<uint16_t>(name.size());
    stream.write(&size, sizeof(size));
    stream.write(name.data(), size);

    while (true)
    {
        stream.read(&packetId, sizeof(uint16_t));
        stream.read(&size, sizeof(uint16_t));
        buf.resize(size);
        stream.read(buf.data(), size);
        std::string reason{buf.begin(), buf.end()};
        std::cout << "DISCONNECTED: " << reason << std::endl;
        stream.disconnect();
        break;
    }
    return 0;
}
