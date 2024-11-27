#include "plugin.hpp"
#include "../plugin.hpp"
#include <cubos/engine/fixed_step/plugin.hpp>
#include "../../game_server/plugin.hpp"
#include <common/packets/plugin.hpp>
#include <common/packets/packet.hpp>
#include <common/packets/receive.hpp>
#include <common/packets/send.hpp>
#include "incoming/join/plugin.hpp"
#include "outgoing/disconnect/plugin.hpp"

CUBOS_DEFINE_TAG(airships::server::incomingPacketHandlerTag);
CUBOS_DEFINE_TAG(airships::server::outgoingPacketHandlerTag);

void airships::server::packetHandlersPlugin(cubos::engine::Cubos& cubos)
{
    cubos.depends(gameServerPlugin);
    cubos.depends(networkPlugin);
    cubos.depends(cubos::engine::fixedStepPlugin);

    cubos.tag(incomingPacketHandlerTag)
        .tagged(cubos::engine::fixedStepTag)
        .after(networkReceiveTag)
        .before(serverTickTag);
    cubos.tag(outgoingPacketHandlerTag).tagged(cubos::engine::fixedStepTag).after(serverTickTag).before(networkSendTag);

    cubos.plugin(joinPacketHandlerPlugin);
    cubos.plugin(serverDisconnectPacketHandlerPlugin);
}
