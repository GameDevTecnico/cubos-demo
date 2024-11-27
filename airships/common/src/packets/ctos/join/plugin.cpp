#include <common/packets/ctos/join/plugin.hpp>
#include <common/packets/ctos/join/pipe.hpp>

void airships::common::joinPacketPlugin(cubos::engine::Cubos& cubos)
{
    cubos.resource<JoinPacketPipe>();
}
