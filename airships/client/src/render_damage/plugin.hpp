#pragma once

#include <cubos/engine/prelude.hpp>
#include <cubos/engine/assets/asset.hpp>
#include <cubos/engine/voxels/grid.hpp>

#include <cubos/core/reflection/external/map.hpp>

namespace airships::client
{
    /// @brief Component which changes the entity's appearance based on its interpolated counterpart health.
    struct RenderDamage
    {
        CUBOS_REFLECT;

        /// @brief Grid to use when below the given health.
        std::map<int, cubos::engine::Asset<cubos::engine::VoxelGrid>> grids;

        /// @brief Offset to apply to the grid.
        glm::vec3 offset;
    };

    void renderDamagePlugin(cubos::engine::Cubos& cubos);
} // namespace airships::client
