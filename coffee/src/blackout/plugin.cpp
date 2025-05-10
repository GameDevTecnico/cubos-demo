#include "plugin.hpp"
#include "../round_manager/plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/engine/render/lights/plugin.hpp>
#include <cubos/engine/render/lights/point.hpp>
#include <cubos/engine/audio/plugin.hpp>
#include <cubos/engine/transform/position.hpp>

#include <glm/glm.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(coffee::BlackoutDestroy)
{
    return cubos::core::ecs::TypeBuilder<BlackoutDestroy>("coffee::BlackoutDestroy").build();
}

CUBOS_REFLECT_IMPL(coffee::Semaphore)
{
    return cubos::core::ecs::TypeBuilder<Semaphore>("coffee::Semaphore")
        .withField("stage", &Semaphore::stage)
        .withField("intensity", &Semaphore::intensity)
        .build();
}

void coffee::blackoutPlugin(Cubos& cubos)
{
    cubos.depends(lightsPlugin);
    cubos.depends(roundManagerPlugin);

    cubos.component<BlackoutDestroy>();
    cubos.component<Semaphore>();

    cubos.system("animate blackout and semaphores")
        .call([](Commands cmds, Query<const WaitingRoundStart&> waitingStates, const GameRoundSettings& settings,
                 Query<Entity, const BlackoutDestroy&> query, Query<Entity, const Semaphore&, PointLight&> semaphores,
                 const DeltaTime& dt) {
            for (auto [waitingState] : waitingStates)
            {
                float ratio = waitingState.time / settings.maxTimeBetweenRounds;

                int stage = 0;
                if (ratio > 0.25F)
                {
                    stage = 1;
                }

                if (ratio > 0.85F)
                {
                    bool destroyed = false;

                    for (auto [ent, _] : query)
                    {
                        destroyed = true;
                        cmds.destroy(ent);
                    }

                    for (auto [ent, _1, _2] : semaphores)
                    {
                        cmds.destroy(ent);
                    }

                    if (destroyed)
                    {
                        AudioSource source{};
                        source.sound = AnyAsset{"bc80cc02-6b19-492e-a993-b5b500716c4e"};
                        source.gain = 2.0F;
                        cmds.create().named("zap").add(source).add(Position{}).add(AudioPlay{});
                    }
                }
                else
                {
                    for (auto [ent, semaphore, light] : semaphores)
                    {
                        float targetIntensity = 0.0F;
                        if (semaphore.stage == stage)
                        {
                            targetIntensity = semaphore.intensity;
                        }

                        float halfTime = 0.025F;
                        float alpha = 1.0F - glm::pow(0.5F, dt.value() / halfTime);
                        light.intensity = glm::mix(light.intensity, targetIntensity, alpha);
                    }
                }
            }
        });
}
