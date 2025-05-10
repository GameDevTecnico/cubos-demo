#include "plugin.hpp"
#include "../round_manager/plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/engine/render/lights/plugin.hpp>
#include <cubos/engine/render/lights/point.hpp>

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
                    for (auto [ent, _] : query)
                    {
                        cmds.destroy(ent);
                    }

                    for (auto [ent, _1, _2] : semaphores)
                    {
                        cmds.destroy(ent);
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
