#pragma once

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/assets/asset.hpp>
#include <cubos/engine/prelude.hpp>
#include <cubos/engine/voxels/grid.hpp>

namespace demo
{
    /// @brief Component for the clock cursor which should be rotated as the game progresses.
    struct Clock
    {
        CUBOS_REFLECT;

        /// @brief Time which the cursor takes to rotate half of the way to the target rotation.
        float halfRotationTime = 1.0F;
    };

    /// @brief Component for the day counters which should be incremented as the game progresses.
    struct DayCounter
    {
        CUBOS_REFLECT;

        /// @name Digit voxel grid assets.
        /// @{
        cubos::engine::Asset<cubos::engine::VoxelGrid> digit0;
        cubos::engine::Asset<cubos::engine::VoxelGrid> digit1;
        cubos::engine::Asset<cubos::engine::VoxelGrid> digit2;
        cubos::engine::Asset<cubos::engine::VoxelGrid> digit3;
        cubos::engine::Asset<cubos::engine::VoxelGrid> digit4;
        cubos::engine::Asset<cubos::engine::VoxelGrid> digit5;
        cubos::engine::Asset<cubos::engine::VoxelGrid> digit6;
        cubos::engine::Asset<cubos::engine::VoxelGrid> digit7;
        cubos::engine::Asset<cubos::engine::VoxelGrid> digit8;
        cubos::engine::Asset<cubos::engine::VoxelGrid> digit9;
        /// @}
    };

    /// @brief Component which regulates the divisor of a day counter.
    struct DayCounterDivisor
    {
        CUBOS_REFLECT;

        /// @brief Divisor of the day counter.
        int divisor = 1;
    };

    void displaysPlugin(cubos::engine::Cubos& cubos);
} // namespace demo
