#include "plugin.hpp"
#include "../round_manager/plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(coffee::BlackoutDestroy)
{
    return cubos::core::ecs::TypeBuilder<BlackoutDestroy>("coffee::BlackoutDestroy").build();
}

void coffee::blackoutPlugin(Cubos& cubos)
{
    cubos.depends(roundManagerPlugin);

    cubos.component<BlackoutDestroy>();

    cubos.system("animate blackout")
        .call([](Commands cmds, Query<const WaitingRoundStart&> waitingStates, const GameRoundSettings& settings,
                 Query<Entity, const BlackoutDestroy&> query) {
            for (auto [waitingState] : waitingStates)
            {
                if (waitingState.time > settings.maxTimeBetweenRounds * 0.75F)
                {
                    for (auto [ent, _] : query)
                    {
                        cmds.destroy(ent);
                    }
                }
            }
        });
}
