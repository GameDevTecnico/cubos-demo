#include "plugin.hpp"
#include <cubos/engine/fixed_step/fixed_delta_time.hpp>
#include <cubos/engine/fixed_step/plugin.hpp>
#include "../game_server/plugin.hpp"
#include "../game_server/game_server.hpp"
#include "events.hpp"
#include "network_client.hpp"
#include "in/join.hpp"

using namespace cubos::engine;
using cubos::core::net::TcpStream;

CUBOS_DEFINE_TAG(airships::server::acceptConnectionTag);
CUBOS_DEFINE_TAG(airships::server::networkStartTag);
CUBOS_DEFINE_TAG(airships::server::inPacketHandlerTag);
CUBOS_DEFINE_TAG(airships::server::outPacketHandlerTag);
CUBOS_DEFINE_TAG(airships::server::networkEndTag);

void airships::server::networkPlugin(Cubos& cubos)
{
    cubos.depends(gameServerPlugin);
    cubos.depends(fixedStepPlugin);

    cubos.tag(networkStartTag).tagged(fixedStepTag).after(serverTickStartTag).before(serverTickTag);
    cubos.tag(networkEndTag).tagged(fixedStepTag).after(serverTickTag).before(serverTickEndTag);
    cubos.tag(acceptConnectionTag).tagged(fixedStepTag).after(serverTickStartTag).before(networkStartTag);
    cubos.tag(inPacketHandlerTag).tagged(fixedStepTag).after(networkStartTag).before(serverTickTag);
    cubos.tag(outPacketHandlerTag).tagged(fixedStepTag).after(serverTickTag).before(networkEndTag);

    cubos.component<NetworkClient>();

    cubos.event<PacketReceiveEvent>();
    cubos.event<PacketSendEvent>();

    /// [Packet Handlers]
    cubos.plugin(joinPacketPlugin);
    /// [Packet Handlers]

    cubos.system("accept incoming connections")
        .tagged(acceptConnectionTag)
        .call([](Commands cmds, const GameServer& server) {
            TcpStream stream;
            while (server.listener().accept(stream))
            {
                stream.setBlocking(false);
                auto client = std::make_shared<TcpStream>(std::move(stream));
                cmds.create().add(NetworkClient{.stream = std::make_unique<TcpStream>(std::move(stream))});
                CUBOS_WARN("Connection accepted!");
            }
        });

    cubos.system("process incoming packets")
        .tagged(networkStartTag)
        .call([](EventWriter<PacketReceiveEvent> writer, Query<Entity, const NetworkClient&> clients) {
            CUBOS_INFO("STARTED PACKET PROCESSING");
            for (auto [ent, client] : clients)
            {
                uint16_t packetId;
                if (!client.stream->readExact(&packetId, sizeof(packetId)))
                {
                    continue;
                }
                uint16_t packetSize;
                if (!client.stream->readExact(&packetSize, sizeof(packetSize)))
                {
                    continue;
                }
                std::vector<uint8_t> buf;
                buf.resize(packetSize);
                if (!client.stream->readExact(buf.data(), packetSize))
                {
                    continue;
                }
                writer.push({.id = packetId, .buf = buf, .entity = ent});
            }
        });

    cubos.system("process outgoing packets")
        .tagged(networkEndTag)
        .call([](EventReader<PacketSendEvent> reader, Query<const NetworkClient&> clients) {
            for (auto& packet : reader)
            {
                for (auto [client] : clients.pin(0, packet.entity))
                {
                    uint16_t id = packet.id;
                    client.stream->writeExact(&id, sizeof(uint16_t));
                    uint16_t size = static_cast<uint16_t>(packet.buf.size());
                    client.stream->writeExact(&size, sizeof(uint16_t));
                    client.stream->writeExact(packet.buf.data(), size);
                }
            }
            CUBOS_INFO("ENDED PACKET PROCESSING");
        });
}
