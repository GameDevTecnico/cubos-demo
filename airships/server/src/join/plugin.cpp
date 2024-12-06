#include "plugin.hpp"
#include "../player/plugin.hpp"
#include "../player/player.hpp"
#include "../game_server/plugin.hpp"
#include <common/packets/ctos/join.hpp>
#include <common/packets/stoc/disconnect.hpp>
#include <common/packets/plugin.hpp>

void airships::server::joinPlugin(cubos::engine::Cubos& cubos)
{
    cubos.depends(gameServerPlugin);
    cubos.depends(playerPlugin);
    cubos.depends(common::packetsPlugin);

    cubos.system("player join")
        .tagged(serverTickTag)
        .call([](cubos::engine::Commands cmds, cubos::engine::EventReader<common::JoinPacket> reader,
                 cubos::engine::Query<Player&> players,
                 cubos::engine::EventWriter<common::ServerDisconnectPacket> writer) {
            for (auto& packet : reader)
            {
                bool validName = true;
                for (auto [other] : players)
                {
                    if (other.name == packet.name)
                    {
                        validName = false;
                        break;
                    }
                }
                if (!validName)
                {
                    // writer.push({.reason = "Player with the same name already connected!", .stream = packet.stream});
                }
                else
                {
                    CUBOS_WARN("Player joined: {}", packet.name);
                    // cmds.create().add(Player{.name = packet.name, .stream = packet.stream});
                }
            }
        });
}
