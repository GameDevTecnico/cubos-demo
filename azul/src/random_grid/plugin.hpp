#pragma once

#include <cubos/engine/prelude.hpp>
#include <cubos/engine/voxels/grid.hpp>
#include <cubos/engine/assets/asset.hpp>

#include <vector>

namespace demo
{
    /// @brief Component which changes the voxel grid being rendered to a randomly selected one.
    ///
    /// Adds a @ref RenderVoxelGrid component if not already present.
    struct RandomGrid
    {
        CUBOS_REFLECT;

        /// @brief Models to choose from.
        std::vector<cubos::engine::Asset<cubos::engine::VoxelGrid>> models;

        /// @brief Offset for the grid.
        glm::vec3 offset{};
    };

    void randomGridPlugin(cubos::engine::Cubos& cubos);
} // namespace demo
