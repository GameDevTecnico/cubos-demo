#pragma once

#include <cubos/engine/prelude.hpp>
#include <cubos/engine/voxels/grid.hpp>
#include <cubos/engine/assets/asset.hpp>

#include <vector>
#include <cmath>

namespace airships::client
{
    /// @brief Component which changes the voxel grid being rendered on an entity periodically.
    ///
    /// Adds a @ref RenderVoxelGrid component if not already present.
    struct Animation
    {
        CUBOS_REFLECT;

        /// @brief List of models to cycle through.
        std::vector<cubos::engine::Asset<cubos::engine::VoxelGrid>> models;

        /// @brief Offset to apply to the render voxel grid.
        glm::vec3 offset = {0.0F, 0.0F, 0.0F};

        /// @brief Seconds between each frame.
        float timeBetweenFrames = 0.5F;

        /// @brief Index of the current model.
        size_t currentModel = 0;

        /// @brief Time since the last frame change.
        float timeSinceLastFrame = INFINITY;
    };

    void animationPlugin(cubos::engine::Cubos& cubos);
} // namespace airships::client
