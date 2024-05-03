#include "plugin.hpp"
#include "../interaction/plugin.hpp"
#include "../progression/plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(demo::Skip)
{
    return cubos::core::ecs::TypeBuilder<Skip>("demo::Skip").build();
}

void demo::skipPlugin(Cubos& cubos)
{
    cubos.depends(interactionPlugin);
    cubos.depends(progressionPlugin);

    cubos.component<Skip>();

    cubos.observer("handle interactions with skip machines")
        .onAdd<Interaction>()
        .call([](Commands cmds, Progression& progression, Query<const Interaction&, const Skip&> skipInteractions) {
            for (auto [interaction, skip] : skipInteractions)
            {
                if (progression.timeOfDay < progression.dayDuration * 0.95F)
                {
                    progression.timeOfDay = progression.dayDuration * 0.95F;
                }
            }
        });
}
