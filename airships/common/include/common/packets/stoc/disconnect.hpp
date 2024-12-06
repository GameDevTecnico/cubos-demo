#pragma once

#include <cubos/core/reflection/reflect.hpp>
#include <string>

namespace airships::common
{
    struct ServerDisconnectPacket
    {
        CUBOS_REFLECT;

        std::string reason;
    };
} // namespace airships::common
