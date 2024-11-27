#include "plugin.hpp"
#include <common/packets/plugin.hpp>
#include <common/packets/packet.hpp>
#include <common/packets/receive.hpp>
#include <common/packets/send.hpp>
#include <common/packets/stoc/disconnect/plugin.hpp>
#include <common/packets/stoc/disconnect/packet.hpp>
#include <common/packets/stoc/disconnect/pipe.hpp>
#include "../../plugin.hpp"

void airships::server::serverDisconnectPacketHandlerPlugin(cubos::engine::Cubos& cubos)
{
    cubos.depends(packetHandlersPlugin);
    cubos.depends(common::packetsPlugin);

    static std::size_t readIndex{0};
    static std::unique_ptr<cubos::core::ecs::EventReader<common::ServerDisconnectPacket>> reader;
    static std::unique_ptr<cubos::core::ecs::EventWriter<common::PacketData>> writer;

    cubos.startupSystem("setup server disconnect packet handler")
        .call([](common::ServerDisconnectPacketPipe& disconnectPipe, common::PacketSendPipe& sendPipe) {
            reader = disconnectPipe.createReader(readIndex);
            writer = sendPipe.createWriter();
        });

    cubos.system("process disconnect packet").tagged(outgoingPacketHandlerTag).call([]() {
        const common::ServerDisconnectPacket* packet;
        CUBOS_INFO("Handling join packet");
        do
        {
            packet = reader->read();
            if (packet != nullptr)
            {
                std::vector<uint8_t> vec{packet->reason.begin(), packet->reason.end()};
                writer->push({.id = common::ServerDisconnectPacket::PacketId, .buffer = vec, .stream = packet->stream});
            }
        } while (packet != nullptr);
    });
}
