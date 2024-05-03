#include "plugin.hpp"
#include "../interaction/plugin.hpp"
#include "../progression/plugin.hpp"
#include "../object/plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/string.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(demo::Skip)
{
    return cubos::core::ecs::TypeBuilder<Skip>("demo::Skip")
        .withField("reward", &Skip::reward)
        .withField("root", &Skip::root)
        .build();
}

void demo::skipPlugin(Cubos& cubos)
{
    cubos.depends(assetsPlugin);
    cubos.depends(transformPlugin);
    cubos.depends(objectPlugin);
    cubos.depends(interactionPlugin);
    cubos.depends(progressionPlugin);

    cubos.component<Skip>();

    cubos.observer("handle interactions with skip machines")
        .onAdd<Interaction>()
        .call([](Commands cmds, Assets& assets, Progression& progression,
                 Query<const Interaction&, const Skip&, const Object&, const ChildOf&, Entity> skipInteractions) {
            for (auto [interaction, skip, skipObject, _, mapEnt] : skipInteractions)
            {
                if (progression.timeOfDay < progression.dayDuration * 0.75F)
                {
                    // Give a reward.
                    for (int i = 1; i <= progression.daysSurvived + 1; ++i)
                    {
                        auto rewardEnt = cmds.spawn(assets.read(skip.reward)->blueprint).entity(skip.root);
                        cmds.relate(rewardEnt, mapEnt, ChildOf{})
                            .add(rewardEnt, Object{.position = skipObject.position + glm::ivec2{1, 0}});
                    }
                }

                if (progression.timeOfDay < progression.dayDuration * 0.95F)
                {
                    progression.timeOfDay = progression.dayDuration * 0.95F;
                }
            }
        });
}
