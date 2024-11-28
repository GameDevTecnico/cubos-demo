#include "plugin.hpp"
#include <cubos/engine/fixed_step/plugin.hpp>
#include <common/packets/plugin.hpp>
#include <common/packets/packet.hpp>
#include "../game_server/plugin.hpp"
#include "../game_server/game_server.hpp"
#include "in/join/plugin.hpp"
#include "out/disconnect/plugin.hpp"

CUBOS_DEFINE_TAG(airships::server::packetReceiverTag);
CUBOS_DEFINE_TAG(airships::server::inPacketHandlerTag);
CUBOS_DEFINE_TAG(airships::server::outPacketHandlerTag);
CUBOS_DEFINE_TAG(airships::server::packetSenderTag);

void airships::server::packetsPlugin(cubos::engine::Cubos& cubos)
{
    cubos.depends(gameServerPlugin);
    cubos.depends(cubos::engine::fixedStepPlugin);
    cubos.depends(common::packetsPlugin);

    cubos.tag(packetReceiverTag).tagged(cubos::engine::fixedStepTag).after(serverTickStartTag).before(serverTickTag);
    cubos.tag(packetSenderTag).tagged(cubos::engine::fixedStepTag).after(serverTickTag).before(serverTickEndTag);
    cubos.tag(inPacketHandlerTag).tagged(cubos::engine::fixedStepTag).after(packetReceiverTag).before(serverTickTag);
    cubos.tag(outPacketHandlerTag).tagged(cubos::engine::fixedStepTag).after(serverTickTag).before(packetSenderTag);

    // [Packer handlers plugins]
    cubos.plugin(inJoinPacketHandlerPlugin);
    cubos.plugin(outDisconnectPacketPlugin);
    // [Packer handlers plugins]

    cubos.system("process incoming packets")
        .tagged(packetReceiverTag)
        .call([](const GameServer& server, cubos::engine::EventWriter<common::PacketReceiveEvent> writer) {
            CUBOS_INFO("STARTED PACKET PROCESSING");
            for (std::shared_ptr<cubos::core::net::TcpStream> client : server.clients())
            {
                uint16_t packetId;
                if (!client->readExact(&packetId, sizeof(packetId)))
                {
                    continue;
                }
                uint16_t packetSize;
                if (!client->readExact(&packetSize, sizeof(packetSize)))
                {
                    continue;
                }
                std::vector<uint8_t> buf;
                buf.resize(packetSize);
                if (!client->readExact(buf.data(), packetSize))
                {
                    continue;
                }
                writer.push({.id = packetId, .buffer = buf, .source = client}, packetId);
            }
        });

    cubos.system("process outgoing packets")
        .tagged(packetSenderTag)
        .call([](cubos::engine::EventReader<common::PacketSendEvent> reader) {
            for (auto& packet : reader)
            {
                uint16_t id = packet.id;
                packet.dest->writeExact(&id, sizeof(uint16_t));
                uint16_t size = static_cast<uint16_t>(packet.buffer.size());
                packet.dest->writeExact(&size, sizeof(uint16_t));
                packet.dest->writeExact(packet.buffer.data(), size);
            }
            CUBOS_INFO("ENDED PACKET PROCESSING");
        });
}
