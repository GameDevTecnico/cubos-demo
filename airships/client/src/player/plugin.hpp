#pragma once

#include "../animation/plugin.hpp"

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/prelude.hpp>
#include <cubos/engine/transform/plugin.hpp>

namespace airships::client
{
    struct Player
    {
        CUBOS_REFLECT;

        std::string horizontalAxis{"horizontal"};
        std::string verticalAxis{"vertical"};
        std::string interactAction{"interact"};

        /// @brief The position where a holdable item will be placed when a player picks it up
        cubos::engine::Position holdablePos{};

        bool initialized{false};
        cubos::engine::Entity interactingWith{};

        float interactDistance{1.0F};

        glm::vec3 direction{0.0f, 0.0f, 1.0f};
        float moveSpeed{5.0f};
        float halfRotationTime{0.1F};
        glm::ivec2 facing = {0, 1};
        glm::quat rot = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f));
    };

    struct PlayerSkin
    {
        CUBOS_REFLECT;

        cubos::engine::Asset<Animation> idleAnimation{};
        cubos::engine::Asset<Animation> walkAnimation{};
    };

    void playerPlugin(cubos::engine::Cubos& cubos);
} // namespace airships::client
