#pragma once

#include <string>

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/prelude.hpp>

namespace demo
{
    /// @brief Component used to allow the player to control their character.
    struct PlayerController
    {
        CUBOS_REFLECT;

        /// @brief Player number.
        int player = 1;

        /// @brief Axis used to move the character in the x direction.
        std::string moveX = "move-x";

        /// @brief Axis used to move the character in the y direction.
        std::string moveY = "move-y";
    };

    void playerControllerPlugin(cubos::engine::Cubos& cubos);
} // namespace demo
