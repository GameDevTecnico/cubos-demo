#pragma once

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/prelude.hpp>

namespace demo
{
    /// @brief Component added to an entity when a player interacts with it.
    struct Interaction
    {
        CUBOS_REFLECT;

        /// @brief Player entity.
        cubos::engine::Entity entity{};
    };

    void interactionPlugin(cubos::engine::Cubos& cubos);
} // namespace demo
