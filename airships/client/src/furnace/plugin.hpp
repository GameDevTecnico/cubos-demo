#pragma once

#include <cubos/engine/prelude.hpp>

#include <string>

namespace airships::client
{
    /// @brief Component which makes an entity the furnace of its parent drivable entity.
    ///
    /// Assumes that the entity has a parent entity with a @ref Drivable component.
    /// Assumes that the entity has a parent entity with a @ref PhysicsBundle component.
    struct Furnace
    {
        CUBOS_REFLECT;

        /// @brief The current coal level of the furnace
        float coal = 10.0F;

        /// @brief The amount of coal added when the player interacts
        float addCoal = 2.5F;

        /// @brief The maximum coal the furnace can take
        float maximumCoal = 10.0F;

        /// @brief The maximum velocity up
        float maxUpVelocity = 20.0F;

        /// @brief The maximum velocity down
        float maxVelocityDown = -10.0F;
    };

    void furnacePlugin(cubos::engine::Cubos& cubos);
} // namespace airships::client