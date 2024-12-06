#include <common/packets/ctos/join.hpp>
#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/string.hpp>

using namespace airships::common;

CUBOS_REFLECT_IMPL(JoinPacket)
{
    return cubos::core::ecs::TypeBuilder<JoinPacket>("airships::common::JoinPacket")
        .withField("name", &JoinPacket::name)
        .build();
}
