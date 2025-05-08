#pragma once

#include <cubos/engine/prelude.hpp>

namespace coffee
{
    struct ToiletPaper
    {
        CUBOS_REFLECT;

        bool attached = false;
    };

    struct MakeOrphan
    {
        CUBOS_REFLECT;
    };

    void toiletPaperPlugin(cubos::engine::Cubos& cubos);
} // namespace coffee