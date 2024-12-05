#pragma once

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/engine/prelude.hpp>

namespace airships::client
{
    /// @brief Component which makes an entity target for interactions.
    struct Interactable
    {
        CUBOS_REFLECT;
    };

    /// @brief Component added to entities with the @ref Interactable component when they are being interacted with
    struct Interaction
    {
        CUBOS_REFLECT;

        /// @brief Player entity which is interacting with the interactable entity
        cubos::engine::Entity player;
    };

    void interactablePlugin(cubos::engine::Cubos& cubos);
} // namespace airships::client