#include "plugin.hpp"
#include "../game_server/plugin.hpp"
#include "../game_server/game_server.hpp"
#include <cubos/engine/fixed_step/plugin.hpp>
#include <common/packets/plugin.hpp>
#include <common/packets/receive.hpp>
#include <common/packets/send.hpp>
#include <algorithm>
#include "packet_handlers/plugin.hpp"

CUBOS_DEFINE_TAG(airships::server::networkReceiveTag);
CUBOS_DEFINE_TAG(airships::server::networkSendTag);

namespace
{
    struct State
    {
        CUBOS_ANONYMOUS_REFLECT(State);

        State(airships::common::PacketReceivePipe& receivePipe, airships::common::PacketSendPipe& sendPipe)
            : writer(receivePipe.createWriter()), reader(sendPipe.createReader(mReadIndex))
        {
        }

        std::unique_ptr<cubos::engine::EventWriter<airships::common::PacketData>> writer;
        std::unique_ptr<cubos::engine::EventReader<airships::common::PacketData>> reader;
        std::size_t mReadIndex{0};
    };
} // namespace

void airships::server::networkPlugin(cubos::engine::Cubos& cubos)
{
    cubos.depends(gameServerPlugin);
    cubos.depends(cubos::engine::fixedStepPlugin);
    cubos.depends(common::packetsPlugin);

    cubos.tag(networkReceiveTag).tagged(cubos::engine::fixedStepTag).after(serverTickStartTag).before(serverTickTag);
    cubos.tag(networkSendTag).tagged(cubos::engine::fixedStepTag).after(serverTickTag).before(serverTickEndTag);

    cubos.plugin(packetHandlersPlugin);

    cubos.uninitResource<State>();

    cubos.startupSystem("setup network pipes")
        .call([](cubos::engine::Commands cmds, common::PacketReceivePipe& receivePipe,
                 common::PacketSendPipe& sendPipe) { cmds.emplaceResource<State>(receivePipe, sendPipe); });

    cubos.system("process incoming network requests")
        .tagged(networkReceiveTag)
        .call([](GameServer& server, State& state) {
            CUBOS_INFO("STARTED NETWORK");
            // Accept all incoming connections
            cubos::core::net::TcpStream stream;
            while (server.listener().accept(stream))
            {
                stream.setBlocking(false);
                auto client = std::make_shared<cubos::core::net::TcpStream>(std::move(stream));
                server.connect(std::move(client));
                CUBOS_INFO("Client connected!");
            }

            // Read all incoming packets from connected clients
            for (std::shared_ptr<cubos::core::net::TcpStream> client : server.clients())
            {
                uint16_t packetId;
                if (client->readExact(&packetId, sizeof(packetId)))
                {
                    uint16_t packetSize;
                    if (client->readExact(&packetSize, sizeof(packetSize)))
                    {
                        std::vector<uint8_t> buf;
                        buf.resize(packetSize);
                        if (client->readExact(buf.data(), packetSize))
                        {
                            state.writer->push({.buffer = buf, .stream = std::move(client)}, packetId);
                        }
                    }
                }
            }
        });

    cubos.system("send outgoing network requests").tagged(networkSendTag).call([](GameServer& server, State& state) {
        // send packets in pipe
        const common::PacketData* packet;
        do
        {
            packet = state.reader->read();
            if (packet != nullptr)
            {
                uint16_t id = packet->id;
                packet->stream->writeExact(&id, sizeof(uint16_t));
                uint16_t size = static_cast<uint16_t>(packet->buffer.size());
                packet->stream->writeExact(&size, sizeof(uint16_t));
                packet->stream->writeExact(packet->buffer.data(), size);
            }
        } while (packet != nullptr);

        // erase disconnected clients
        std::vector<std::shared_ptr<cubos::core::net::TcpStream>>& clients = server.clients();
        std::vector<std::shared_ptr<cubos::core::net::TcpStream>> filtered{};
        std::copy_if(clients.begin(), clients.end(), std::back_inserter(filtered),
                     [](auto& client) { return !client->eof(); });
        server.clients() = filtered;

        CUBOS_INFO("ENDED NETWORK");
    });
}
