#pragma once

#include <cubos/core/reflection/reflect.hpp>

namespace demo
{
    struct [[cubos::component("")]] Dead
    {
        CUBOS_REFLECT;
    };
} // namespace demo