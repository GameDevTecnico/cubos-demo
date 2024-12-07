#pragma once

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/prelude.hpp>

#include <cmath>

namespace airships::client
{
    /// @brief Component which specifies that an entity should be hidden if a ray projected from a camera intersects
    /// it.
    struct SeeThrough
    {
        CUBOS_REFLECT;

        /// @brief If the distance from the camera to the entity is larger than this value, the entity is always
        /// visible.
        float maxDistance{INFINITY};
    };

    /// @brief Component for cameras which hide SeeThrough entities.
    struct SeeThroughCamera
    {
        CUBOS_REFLECT;

        /// @brief Collision mask to use when raycasting for SeeThrough entities.
        uint64_t mask{0};
    };

    void seeThroughPlugin(cubos::engine::Cubos& cubos);
} // namespace airships::client
