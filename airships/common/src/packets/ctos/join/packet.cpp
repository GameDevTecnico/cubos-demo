#include <common/packets/ctos/join/packet.hpp>
#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/ecs/reflection.hpp>

using namespace airships::common;

CUBOS_REFLECT_IMPL(JoinPacket)
{
    return cubos::core::ecs::TypeBuilder<JoinPacket>("airships::common::JoinPacket")
        .withField("name", &JoinPacket::name)
        .build();
}
