#include "plugin.hpp"
#include "player.hpp"
#include "../game_server/plugin.hpp"
#include <common/packets/ctos/join/pipe.hpp>
#include <common/packets/ctos/join/plugin.hpp>
#include <common/packets/ctos/join/packet.hpp>
#include <common/packets/stoc/disconnect/pipe.hpp>
#include <common/packets/stoc/disconnect/plugin.hpp>
#include <common/packets/stoc/disconnect/packet.hpp>

void airships::server::playerPlugin(cubos::engine::Cubos& cubos)
{
    cubos.depends(gameServerPlugin);
    cubos.depends(common::joinPacketPlugin);
    cubos.depends(common::serverDisconnectPacketPlugin);

    cubos.component<Player>();

    static std::size_t readIndex{0};
    static std::unique_ptr<cubos::engine::EventReader<common::JoinPacket>> reader;

    static std::unique_ptr<cubos::engine::EventWriter<common::ServerDisconnectPacket>> disconnectWriter;

    cubos.startupSystem("").call([](common::JoinPacketPipe& pipe, common::ServerDisconnectPacketPipe& discPipe) {
        reader = pipe.createReader(readIndex);
        disconnectWriter = discPipe.createWriter();
    });

    cubos.system("add player on join")
        .tagged(serverTickTag)
        .call([](cubos::engine::Commands cmds, cubos::engine::Query<Player&> query) {
            const common::JoinPacket* packet;
            do
            {
                packet = reader->read();
                if (packet != nullptr)
                {
                    const std::string& name = packet->name;
                    bool nameConflict = false;
                    for (auto [other] : query)
                    {
                        if (other.name == name)
                        {
                            nameConflict = true;
                            break;
                        }
                    }
                    if (nameConflict)
                    {
                        CUBOS_ERROR("Player name repeated!");
                        disconnectWriter->push(
                            {.reason = "Player with that name already logged in!", .stream = packet->stream});
                    }
                    else
                    {
                        CUBOS_WARN("Player joined: {}", name);
                        cmds.create().add(Player{.name = packet->name, .stream = packet->stream});
                    }
                }
            } while (packet != nullptr);
        });
}
