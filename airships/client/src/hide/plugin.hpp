#pragma once

#include <cubos/engine/prelude.hpp>
#include <cubos/engine/render/voxels/grid.hpp>

namespace airships::client
{
    /// @brief Component which removes RenderVoxelGrid on addition, and restores it on removal.
    struct Hide
    {
        CUBOS_REFLECT;

        /// @brief Component to be restored when the component is removed.
        cubos::engine::RenderVoxelGrid component;
    };

    void hidePlugin(cubos::engine::Cubos& cubos);
} // namespace airships::client