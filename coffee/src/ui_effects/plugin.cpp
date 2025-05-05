#include "plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/engine/ui/text/plugin.hpp>
#include <cubos/engine/ui/text/text.hpp>

using namespace cubos::engine;

namespace
{
    struct State
    {
        CUBOS_ANONYMOUS_REFLECT(State);

        /// @brief Current state of the cycle.
        float state = 0.0F;
    };
} // namespace

CUBOS_REFLECT_IMPL(coffee::UIBlink)
{
    return cubos::core::ecs::TypeBuilder<UIBlink>("coffee::UIBlink").build();
}

void coffee::uiEffectsPlugin(Cubos& cubos)
{
    cubos.depends(uiTextPlugin);

    cubos.component<UIBlink>();

    cubos.resource<State>();

    cubos.system("update RenderEnvironment")
        .call([](const DeltaTime& dt, State& state, Query<UIText&, UIBlink&> textElements) {
            // Update time
            state.state += 4.0F * dt.value();

            // Update text transparency
            for (auto [text, _] : textElements)
            {
                text.color = glm::vec4(1.0F, 1.0F, 1.0F, 0.5F + 0.5F * glm::sin(state.state));
            }
        });
}
