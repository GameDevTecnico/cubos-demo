#include "plugin.hpp"
#include "../game_server/plugin.hpp"
#include <common/packets/ctos/join/pipe.hpp>
#include <common/packets/ctos/join/plugin.hpp>
#include <cubos/core/reflection/external/string.hpp>

void airships::server::welcomePlugin(cubos::engine::Cubos& cubos)
{
    cubos.depends(gameServerPlugin);
    cubos.depends(common::joinPacketPlugin);

    static std::size_t readIndex{0};
    static std::unique_ptr<cubos::engine::EventReader<common::JoinPacket>> reader;

    cubos.startupSystem("setup welcome plugin").call([](common::JoinPacketPipe& pipe) {
        reader = pipe.createReader(readIndex);
    });

    cubos.system("welcome players who joined").tagged(serverTickTag).call([]() {
        const common::JoinPacket* packet;
        CUBOS_INFO("TICK WELCOME");
        do
        {
            packet = reader->read();
            if (packet != nullptr)
            {
                CUBOS_WARN("New player joined: {}", packet->name);
            }
        } while (packet != nullptr);
    });
}
