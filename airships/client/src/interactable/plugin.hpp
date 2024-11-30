#pragma once

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/engine/prelude.hpp>

namespace airships::client
{
    struct Interactable
    {
        CUBOS_REFLECT;
    };

    void interactablePlugin(cubos::engine::Cubos& cubos);
} // namespace airships::client