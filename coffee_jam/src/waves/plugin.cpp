#include "plugin.hpp"
#include "../zombie/plugin.hpp"
#include "../progression/plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/glm.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(demo::Waves)
{
    return cubos::core::ecs::TypeBuilder<Waves>("demo::Waves")
        .withField("waveZombiesBase", &Waves::waveZombiesBase)
        .withField("waveZombiesIncrease", &Waves::waveZombiesIncrease)
        .withField("wavesPerNight", &Waves::wavesPerNight)
        .withField("zombie", &Waves::zombie)
        .withField("zombieRoot", &Waves::zombieRoot)
        .withField("walker", &Waves::walker)
        .build();
}

void demo::wavesPlugin(Cubos& cubos)
{
    cubos.depends(assetsPlugin);
    cubos.depends(transformPlugin);
    cubos.depends(walkerPlugin);
    cubos.depends(zombiePlugin);
    cubos.depends(progressionPlugin);

    cubos.component<Waves>();

    cubos.system("spawn waves of zombies")
        .call([](Commands cmds, Assets& assets, Progression& progression,
                 Query<const Waves&, const ChildOf&, Entity> waves, Query<const ZombieController&> zombies) {
            if (progression.timeOfDay < progression.dayDuration)
            {
                return;
            }

            if (progression.scoreToFinishNight == -1)
            {
                // Initialize the night score.
                progression.scoreToFinishNight = 0;
                for (auto [wave, _1, _2] : waves)
                {
                    progression.scoreToFinishNight +=
                        (wave.waveZombiesBase + wave.waveZombiesIncrease * progression.daysSurvived) *
                        wave.wavesPerNight;
                }
            }

            if (zombies.empty() && progression.score < progression.scoreToFinishNight)
            {
                // No more zombies and the night is not over yet, spawn a new wave.
                for (auto [wave, _1, mapEnt] : waves)
                {
                    for (int i = 0; i < wave.waveZombiesBase + wave.waveZombiesIncrease * progression.daysSurvived; ++i)
                    {
                        auto zombieEnt = cmds.spawn(assets.read(wave.zombie)->blueprint).entity(wave.zombieRoot);
                        cmds.relate(zombieEnt, mapEnt, ChildOf{});
                        cmds.add(zombieEnt, wave.walker);
                    }
                }
            }
        });

    cubos.observer("increase score on zombie death")
        .onRemove<ZombieController>()
        .call([](Query<> query, Progression& progression) { progression.score += 1; });
}
