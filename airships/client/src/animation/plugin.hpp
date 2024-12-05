#pragma once

#include <cubos/engine/prelude.hpp>
#include <cubos/engine/voxels/grid.hpp>
#include <cubos/engine/assets/asset.hpp>

#include <vector>
#include <cmath>

namespace airships::client
{
    /// @brief Asset which describes an animation.
    struct Animation
    {
        CUBOS_REFLECT;

        /// @brief List of models to cycle through.
        std::vector<cubos::engine::Asset<cubos::engine::VoxelGrid>> models;

        /// @brief Seconds between each frame.
        float timeBetweenFrames = 0.5F;
    };

    /// @brief Component which changes the voxel grid being rendered on an entity periodically.
    ///
    /// Adds a @ref RenderVoxelGrid component if not already present.
    struct RenderAnimation
    {
        CUBOS_REFLECT;

        /// @brief Animation to be played.
        cubos::engine::Asset<Animation> animation;

        /// @brief Cache handles of the current models to keep them loaded.
        std::vector<cubos::engine::Asset<cubos::engine::VoxelGrid>> currentModels;

        /// @brief Offset to apply to the render voxel grid.
        glm::vec3 offset = {0.0F, 0.0F, 0.0F};

        /// @brief Animation speed multiplier.
        float speedMultiplier = 1.0F;

        /// @brief Index of the current model.
        int currentModel = 0;

        /// @brief Time since the last frame change.
        float timeSinceLastFrame = INFINITY;

        inline void reset()
        {
            currentModel = -1;
            timeSinceLastFrame = INFINITY;
        }

        inline void play(cubos::engine::Asset<Animation> animation)
        {
            if (this->animation != animation)
            {
                this->animation = animation;
                this->reset();
            }
        }
    };

    void animationPlugin(cubos::engine::Cubos& cubos);
} // namespace airships::client
