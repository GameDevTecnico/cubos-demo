#include "plugin.hpp"
#include "../../plugin.hpp"
#include <common/packets/plugin.hpp>
#include <common/packets/packet.hpp>
#include <common/packets/ctos/join/packet.hpp>

void airships::server::inJoinPacketHandlerPlugin(cubos::engine::Cubos& cubos)
{
    cubos.depends(packetsPlugin);
    cubos.depends(common::packetsPlugin);

    /// @todo setting the mask in the EventReader is not working, why?
    cubos.system("join packet handler")
        .tagged(inPacketHandlerTag)
        .call([](cubos::engine::EventReader<common::PacketReceiveEvent> reader,
                 cubos::engine::EventWriter<common::JoinPacket> writer) {
            for (auto& packet : reader)
            {
                if (packet.id == common::JoinPacket::PacketId)
                {
                    std::string name{packet.buffer.begin(), packet.buffer.end()};
                    writer.push({.name = name, .stream = packet.source});
                }
            }
        });
}
