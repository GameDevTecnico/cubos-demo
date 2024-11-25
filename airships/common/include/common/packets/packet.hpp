#pragma once

#include <cubos/core/reflection/reflect.hpp>
#include <cubos/core/net/address.hpp>
#include <vector>
#include <cstdint>

namespace airships::common
{
    struct PacketData
    {
        CUBOS_REFLECT;

        std::vector<uint8_t> buffer;
        cubos::core::net::Address address;
    };
} // namespace airships::common
