#include "plugin.hpp"
#include <common/packets/plugin.hpp>
#include <common/packets/packet.hpp>
#include <common/packets/receive.hpp>
#include <common/packets/send.hpp>
#include <common/packets/ctos/join/plugin.hpp>
#include <common/packets/ctos/join/packet.hpp>
#include <common/packets/ctos/join/pipe.hpp>
#include "../../plugin.hpp"

void airships::server::joinPacketHandlerPlugin(cubos::engine::Cubos& cubos)
{
    cubos.depends(packetHandlersPlugin);
    cubos.depends(common::packetsPlugin);

    static std::size_t readIndex{0};
    static std::unique_ptr<cubos::core::ecs::EventReader<common::PacketData, common::JoinPacket::PacketId>> reader;
    static std::unique_ptr<cubos::engine::EventWriter<common::JoinPacket>> writer;

    cubos.startupSystem("setup join packet handler")
        .call([](common::PacketReceivePipe& packetPipe, common::JoinPacketPipe& joinPipe) {
            reader = packetPipe.createReader<common::JoinPacket::PacketId>(readIndex);
            writer = joinPipe.createWriter();
        });

    cubos.system("process join packet").tagged(incomingPacketHandlerTag).call([]() {
        const common::PacketData* packet;
        CUBOS_INFO("Handling join packet");
        do
        {
            packet = reader->read();
            if (packet != nullptr)
            {
                std::string name{packet->buffer.begin(), packet->buffer.end()};
                writer->push({.name = name, .stream = packet->stream});
            }
        } while (packet != nullptr);
    });
}
