#include "plugin.hpp"
#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/glm.hpp>

CUBOS_REFLECT_IMPL(demo::Interactable)
{
    return cubos::core::ecs::TypeBuilder<demo::Interactable>("demo::Iterable").build();
}

void demo::interactablePlugin(cubos::engine::Cubos& cubos)
{
    cubos.component<demo::Interactable>();
}