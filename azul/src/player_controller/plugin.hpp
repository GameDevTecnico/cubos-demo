#pragma once

#include <string>

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/assets/asset.hpp>
#include <cubos/engine/prelude.hpp>
#include <cubos/engine/scene/scene.hpp>

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

        /// @brief Action used to shoot.
        std::string shoot = "shoot";

        /// @brief Asset of the bullet to shoot.
        cubos::engine::Asset<cubos::engine::Scene> bullet;

        /// @brief Bullet reload time.
        float bulletReloadTime{1.0F};

        /// @brief Bullet speed multiplier.
        float bulletSpeed{5.0F};

        /// @brief Minimum distance for shooting.
        int shootMinDistance{3};

        /// @brief Distance at which the shooting cone widens.
        int shootConeWidenDistance{5};

        float bulletReloadAcc{0.0F};
    };

    void playerControllerPlugin(cubos::engine::Cubos& cubos);
} // namespace demo
