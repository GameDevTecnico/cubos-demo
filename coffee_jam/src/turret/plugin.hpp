#pragma once

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/assets/asset.hpp>
#include <cubos/engine/prelude.hpp>
#include <cubos/engine/scene/scene.hpp>

namespace demo
{
    /// @brief Component for turret entities.
    struct Turret
    {
        CUBOS_REFLECT;

        /// @brief Bullet scene to instantiate when shooting.
        cubos::engine::Asset<cubos::engine::Scene> bullet;

        /// @brief Name of the root entity of the bullet scene.
        std::string root;

        /// @brief Time in seconds between shots.
        float cooldown = 1.0F;

        /// @brief Time in seconds since the last shot.
        float timeSinceLastShot = 0.0F;

        /// @brief Time in seconds the turret takes to rotate 180 degrees.
        float rotationTime = 1.0F;

        /// @brief Height at which the bullet is spawned.
        float bulletHeight = 8.0F;

        /// @brief Distance at which the bullet is spawned.
        float bulletDistance = 1.0F;

        /// @brief Speed at which the bullet moves.
        float bulletSpeed = 10.0F;

        /// @brief Maximum time in seconds the bullet can exist.
        float maxBulletTime = 5.0F;

        /// @brief Current ammo count.
        int ammo = 0;

        /// @brief Maximum ammo count to allow reload.
        int maxAmmoForReload;

        /// @brief How much ammo each reload gives.
        int ammoPerReload;
    };

    /// @brief Component for entities which can be used as ammo.
    struct Ammo
    {
        CUBOS_REFLECT;
    };

    /// @brief Component for bullet entities.
    struct Bullet
    {
        CUBOS_REFLECT;

        /// @brief Entity that shot the bullet.
        cubos::engine::Entity shooter;

        /// @brief How fast the bullet moves.
        float speed = 10.0F;

        /// @brief Time in seconds since the bullet was spawned.
        float time = 0.0F;

        /// @brief Maximum time in seconds the bullet can exist.
        float maxTime = 5.0F;
    };

    void turretPlugin(cubos::engine::Cubos& cubos);
} // namespace demo
