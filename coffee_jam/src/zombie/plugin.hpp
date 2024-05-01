#pragma once

#include <string>

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/prelude.hpp>
#include <cubos/engine/assets/asset.hpp>
#include <cubos/engine/assets/assets.hpp>
#include <cubos/engine/voxels/grid.hpp>

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

        /// @brief Last position of the walker, used to check if the walker is stuck.
        glm::ivec2 lastPosition = {-1, -1};
    };

    void zombiePlugin(cubos::engine::Cubos& cubos);
} // namespace demo
