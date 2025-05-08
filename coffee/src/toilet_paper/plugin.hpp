#pragma once

#include <cubos/engine/prelude.hpp>

namespace coffee
{
    struct ToiletPaper
    {
        CUBOS_REFLECT;

        bool attached = false;
    };

    void toiletPaperPlugin(cubos::engine::Cubos& cubos);
} // namespace coffee