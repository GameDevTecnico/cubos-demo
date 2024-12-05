#pragma once

#include <cubos/core/reflection/reflect.hpp>
#include <cubos/engine/prelude.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

namespace airships::client
{
    struct Player
    {
        CUBOS_REFLECT;

        int player{-1};
        std::string horizontalAxis{"horizontal"};
        std::string verticalAxis{"vertical"};

        glm::vec3 direction{0.0f, 0.0f, 1.0f};
        float moveSpeed{5.0f};
        glm::ivec2 facing = {0, 1};
        glm::quat rot = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f));
    };

    void playerPlugin(cubos::engine::Cubos& cubos);
} // namespace airships::client
