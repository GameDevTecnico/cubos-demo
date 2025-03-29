#include "plugin.hpp"
#include "../waves/plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/vector.hpp>

#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/assets/plugin.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(demo::WavesAnimator)
{
    return cubos::core::ecs::TypeBuilder<WavesAnimator>("demo::WavesAnimator")
        .withField("water", &WavesAnimator::water)
        .withField("entities", &WavesAnimator::entities)
        .withField("waterLerpFactor", &WavesAnimator::waterLerpFactor)
        .build();
}

void demo::wavesAnimatorPlugin(Cubos& cubos)
{
    cubos.depends(transformPlugin);
    cubos.depends(assetsPlugin);
    cubos.depends(wavesPlugin);

    cubos.component<WavesAnimator>();

    auto initWavesAnimator = [](Commands cmds, Assets& assets, Query<Entity, const Waves&, WavesAnimator&> query) {
        for (auto [entity, waves, animator] : query)
        {
            auto rootEntity = cmds.create().named("waves").relatedTo(entity, ChildOf{}).entity();

            animator.entities.resize(waves.state.size());
            for (size_t y = 0; y < waves.state.size(); ++y)
            {
                animator.entities[y].resize(waves.state[y].size());

                for (size_t x = 0; x < waves.state[y].size(); ++x)
                {
                    Entity waveEntity =
                        cmds.spawn(*assets.read(animator.water)).add(Position{{x, 0, y}}).entity();
                    cmds.relate(waveEntity, rootEntity, ChildOf{});
                    animator.entities[y][x] = waveEntity;
                }
            }
        }
    };

    cubos.observer("spawn wave entities").onAdd<WavesAnimator>().call(initWavesAnimator);

    cubos.system("animate wave entities")
        .call(
            [](const DeltaTime& dt, Query<WavesAnimator&, const Waves&> animatorQuery, Query<Position&> positionQuery) {
                for (auto [animator, waves] : animatorQuery)
                {
                    for (size_t y = 0; y < waves.state.size(); ++y)
                    {
                        for (size_t x = 0; x < waves.state[y].size(); ++x)
                        {
                            auto match = positionQuery.at(animator.entities[y][x]);
                            auto [position] = *match;
                            position.vec.y = waves.actual[y][x];
                        }
                    }
                }
            });
}
