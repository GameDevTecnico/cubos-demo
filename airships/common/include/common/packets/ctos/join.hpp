#pragma once

#include <cubos/core/reflection/reflect.hpp>
#include <string>

namespace airships::common
{
    struct JoinPacket
    {
        CUBOS_REFLECT;

        std::string name;
    };
} // namespace airships::common
