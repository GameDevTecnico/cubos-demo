#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/vector.hpp>
#include <common/packets/packet.hpp>

using namespace airships::common;

CUBOS_REFLECT_IMPL(PacketData)
{
    return cubos::core::ecs::TypeBuilder<PacketData>("airships::common::PacketData")
        .withField("id", &PacketData::id)
        .withField("buffer", &PacketData::buffer)
        .build();
}
