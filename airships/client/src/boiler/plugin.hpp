#pragma once

#include <cubos/engine/prelude.hpp>

#include <string>

namespace airships::client
{
    /// @brief Component which makes an entity the boiler of its parent drivable entity.
    ///
    /// Assumes that the entity has a parent entity with a @ref Drivable component.
    /// Assumes that the entity has a parent entity with a @ref PhysicsBundle component.
    struct Boiler
    {
        CUBOS_REFLECT;

        /// @brief Index of the player controlling this entity.
        int player = -1;

        /// @brief Input axis used to control the lever.
        std::string leverAxis = "vertical";

        /// @brief Speed at which the lever moves.
        float leverSpeed = 1.0F;

        /// @brief Current position of the lever, in the range [-1, 1].
        float leverPosition = 0.0F;

        /// @brief Target position of the lever, in the range [-1, 1].
        float targetLeverPosition = 0.0F;

        /// @brief Interpolation factor for the lever position.
        float leverLerpFactor = 0.1F;

        /// @brief Maximum buoyancy ratio.
        float maxBuoyancy = 1.5F;

        /// @brief Minimum buoyancy ratio.
        float minBuoyancy = 0.0F;
    };

    struct BoilerLeverCollider
    {
        CUBOS_REFLECT;
    };

    struct BoilerLeverAnimator
    {
        CUBOS_REFLECT;

        float minHeight{-1.0F};
        float maxHeight{1.0F};
    };

    void boilerPlugin(cubos::engine::Cubos& cubos);
} // namespace airships::client