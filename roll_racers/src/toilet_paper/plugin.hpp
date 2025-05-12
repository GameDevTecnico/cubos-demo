#pragma once

#include <cubos/engine/prelude.hpp>

#include <glm/glm.hpp>

namespace roll_racers
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
} // namespace roll_racers