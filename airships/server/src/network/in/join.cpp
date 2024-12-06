#include "join.hpp"
#include "../plugin.hpp"
#include "../../game_server/plugin.hpp"
#include "../events.hpp"
#include "../network_client.hpp"
#include "common/packets/ctos/join.hpp"
#include <common/packets/types.hpp>
#include <cubos/core/data/des/binary.hpp>
#include "../../player/plugin.hpp"
#include "../../player/player.hpp"
#include <common/packets/stoc/disconnect.hpp>

using namespace cubos::engine;
using airships::common::PacketType;
using cubos::core::data::BinaryDeserializer;
using cubos::core::memory::BufferStream;

void airships::server::joinPacketPlugin(Cubos& cubos)
{
    cubos.depends(networkPlugin);
    cubos.depends(gameServerPlugin);
    cubos.depends(playerPlugin);

    cubos.system("process join packet")
        .tagged(inPacketHandlerTag)
        .call([](Commands cmds, EventReader<PacketReceiveEvent> reader,
                 Query<const NetworkClient&, const Player&> players) {
            for (auto& packet : reader)
            {
                if (packet.id == static_cast<uint16_t>(PacketType::JOIN))
                {
                    BufferStream stream{packet.buf.data(), packet.buf.size()};
                    BinaryDeserializer deser{stream};
                    common::JoinPacket joinPacket;
                    if (deser.read(joinPacket))
                    {
                        // Would be better to separate packet deser from logic probably, but this check is need to not
                        // fired join event, probably would need to have two separate events (login and join?)
                        for (auto [client, player] : players)
                        {
                            if (player.name == joinPacket.name)
                            {
                                // send disconnect packet here
                                return;
                            }
                        }
                        cmds.add(packet.entity, Player{.name = joinPacket.name});
                    }
                }
            }
        });
}
