#include "plugin.hpp"
#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/glm.hpp>

CUBOS_REFLECT_IMPL(airships::client::Interactable)
{
    return cubos::core::ecs::TypeBuilder<airships::client::Interactable>("airships::client::Interactable").build();
}

CUBOS_REFLECT_IMPL(airships::client::Interaction)
{
    return cubos::core::ecs::TypeBuilder<airships::client::Interaction>("airships::client::Interaction")
        .withField("player", &Interaction::player)
        .build();
}

void airships::client::interactablePlugin(cubos::engine::Cubos& cubos)
{
    cubos.component<Interactable>();
    cubos.component<Interaction>();
}
