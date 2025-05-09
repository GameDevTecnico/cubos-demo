#pragma once

#include <cubos/engine/prelude.hpp>

#include <glm/glm.hpp>

namespace coffee
{
    struct ToiletPaper
    {
        CUBOS_REFLECT;

        int player = -1;
        glm::vec3 carPosition{};
        float immunityTime{1.0F};
        float heldTime{0.0F};
    };

    void toiletPaperPlugin(cubos::engine::Cubos& cubos);
} // namespace coffee