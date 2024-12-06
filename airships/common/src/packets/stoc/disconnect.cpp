#include <common/packets/stoc/disconnect.hpp>
#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/string.hpp>

using namespace airships::common;

CUBOS_REFLECT_IMPL(ServerDisconnectPacket)
{
    return cubos::core::ecs::TypeBuilder<ServerDisconnectPacket>("airships::common::ServerDisconnectPacket")
        .withField("reason", &ServerDisconnectPacket::reason)
        .build();
}
