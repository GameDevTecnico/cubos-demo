#include <common/packets/stoc/disconnect/packet.hpp>
#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/ecs/reflection.hpp>

using namespace airships::common;

CUBOS_REFLECT_IMPL(ServerDisconnectPacket)
{
    return cubos::core::ecs::TypeBuilder<ServerDisconnectPacket>("airships::common::JoinPacket")
        .withField("reason", &ServerDisconnectPacket::reason)
        .build();
}
