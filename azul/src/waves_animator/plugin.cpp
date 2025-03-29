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
            for (size_t i = 0; i < waves.state.size(); ++i)
            {
                animator.entities[i].resize(waves.state[i].size());

                for (size_t j = 0; j < waves.state[i].size(); ++j)
                {
                    Entity waveEntity =
                        cmds.spawn(*assets.read(animator.water)).add(Position{{j * 8, 0, i * 8}}).entity();
                    cmds.relate(waveEntity, rootEntity, ChildOf{});
                    animator.entities[i][j] = waveEntity;
                }
            }
        }
    };

    cubos.observer("spawn wave entities").onAdd<WavesAnimator>().call(initWavesAnimator);

    cubos.system("animate wave entities")
        .call([](const DeltaTime& dt, Query<const WavesAnimator&, const Waves&> animatorQuery,
                 Query<Position&> positionQuery) {
            for (auto [animator, waves] : animatorQuery)
            {
                for (size_t i = 0; i < waves.state.size(); ++i)
                {
                    for (size_t j = 0; j < waves.state[i].size(); ++j)
                    {
                        auto match = positionQuery.at(animator.entities[i][j]);
                        auto [position] = *match;
                        auto target = static_cast<float>(waves.state[i][j]);

                        position.vec.y =
                            glm::mix(position.vec.y, target, 1 - glm::pow(1.0 - animator.waterLerpFactor, dt.value()));
                    }
                }
            }
        });
}
