#include "plugin.hpp"
#include "../../plugin.hpp"
#include <common/packets/plugin.hpp>
#include <common/packets/packet.hpp>
#include <common/packets/stoc/disconnect/packet.hpp>

void airships::server::outDisconnectPacketPlugin(cubos::engine::Cubos& cubos)
{
    cubos.depends(packetsPlugin);
    cubos.depends(common::packetsPlugin);

    cubos.system("out disconnect packet handler")
        .tagged(inPacketHandlerTag)
        .call([](cubos::engine::EventReader<common::ServerDisconnectPacket> reader,
                 cubos::engine::EventWriter<common::PacketSendEvent> writer) {
            for (auto& packet : reader)
            {
                std::vector<uint8_t> buf{packet.reason.begin(), packet.reason.end()};
                writer.push({.id = common::ServerDisconnectPacket::PacketId, .buffer = buf, .dest = packet.stream});
            }
        });
}
