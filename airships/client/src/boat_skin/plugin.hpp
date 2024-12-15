#pragma once

#include "../render_damage/plugin.hpp"

#include <cubos/engine/prelude.hpp>
#include <cubos/engine/render/voxels/grid.hpp>

namespace airships::client
{
    /// @brief Component which describes a boat skin.
    struct BoatSkin
    {
        CUBOS_REFLECT;

        RenderDamage hull;
        cubos::engine::RenderVoxelGrid balloon;
        cubos::engine::RenderVoxelGrid rudder;
        cubos::engine::RenderVoxelGrid anchor;
    };

    struct BoatSkinHull
    {
        CUBOS_REFLECT;
    };

    struct BoatSkinBalloon
    {
        CUBOS_REFLECT;
    };

    struct BoatSkinRudder
    {
        CUBOS_REFLECT;
    };

    struct BoatSkinAnchor
    {
        CUBOS_REFLECT;
    };

    void boatSkinPlugin(cubos::engine::Cubos& cubos);
} // namespace airships::client