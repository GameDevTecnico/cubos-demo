#pragma once

#include <string>

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/prelude.hpp>
#include <cubos/engine/assets/asset.hpp>
#include <cubos/engine/voxels/grid.hpp>
#include <cubos/engine/scene/scene.hpp>

namespace demo
{
    /// @brief Component used to allow zombie movement.
    struct ZombieController
    {
        CUBOS_REFLECT;

        /// @brief Time until the zombie tries to search for another path.
        float rePathTime = 10.0F;

        /// @brief Time accumulator for the re-path time.
        float rePathTimeAcc = 0.0F;

        /// @brief Time until the zombies can hit the player again.
        float hitTime = 1.0F;

        /// @brief Time accumulator for the hit time.
        float hitTimeAcc = 0.0F;

        /// @brief Item to drop on death.
        cubos::engine::Asset<cubos::engine::Scene> item;

        /// @brief Item drop rate (from 0 to 1).
        float itemRate = 0.5F;

        /// @brief How much a path is a penalized by each damage point needed to traverse it.
        float hpPenalization = 1.0F;
    };

    void zombiePlugin(cubos::engine::Cubos& cubos);
} // namespace demo
