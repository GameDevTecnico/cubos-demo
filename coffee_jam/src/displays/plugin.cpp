#include "plugin.hpp"
#include "../progression/plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/renderer/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(demo::Clock)
{
    return cubos::core::ecs::TypeBuilder<Clock>("demo::Clock")
        .withField("halfRotationTime", &Clock::halfRotationTime)
        .build();
}

CUBOS_REFLECT_IMPL(demo::DayCounter)
{
    return cubos::core::ecs::TypeBuilder<DayCounter>("demo::DayCounter")
        .withField("digit0", &DayCounter::digit0)
        .withField("digit1", &DayCounter::digit1)
        .withField("digit2", &DayCounter::digit2)
        .withField("digit3", &DayCounter::digit3)
        .withField("digit4", &DayCounter::digit4)
        .withField("digit5", &DayCounter::digit5)
        .withField("digit6", &DayCounter::digit6)
        .withField("digit7", &DayCounter::digit7)
        .withField("digit8", &DayCounter::digit8)
        .withField("digit9", &DayCounter::digit9)
        .build();
}

CUBOS_REFLECT_IMPL(demo::DayCounterDivisor)
{
    return cubos::core::ecs::TypeBuilder<DayCounterDivisor>("demo::DayCounterDivisor")
        .withField("divisor", &DayCounterDivisor::divisor)
        .build();
}

void demo::displaysPlugin(Cubos& cubos)
{
    cubos.depends(assetsPlugin);
    cubos.depends(rendererPlugin);
    cubos.depends(transformPlugin);
    cubos.depends(progressionPlugin);

    cubos.component<Clock>();
    cubos.component<DayCounter>();
    cubos.component<DayCounterDivisor>();

    cubos.system("update Clocks")
        .call([](const DeltaTime& dt, const Progression& progression, Query<const Clock&, Rotation&> query) {
            for (auto [clock, rotation] : query)
            {
                float targetAngle;
                if (progression.timeOfDay < progression.dayDuration)
                {
                    targetAngle = glm::radians(180.0F * progression.timeOfDay / progression.dayDuration);
                }
                else if (progression.scoreToFinishNight == -1)
                {
                    targetAngle = glm::radians(180.0F);
                }
                else
                {
                    targetAngle = glm::radians(180.0F + 180.0F * static_cast<float>(progression.score) /
                                                            static_cast<float>(progression.scoreToFinishNight));
                }

                auto targetRotation = glm::angleAxis(-targetAngle, glm::vec3(1.0F, 0.0F, 0.0F));
                auto rotationAlpha = 1.0F - glm::pow(0.5F, dt.value() / clock.halfRotationTime);
                rotation.quat = glm::slerp(rotation.quat, targetRotation, rotationAlpha);
            }
        });

    cubos.system("update DayCounters")
        .call([](const Progression& progression,
                 Query<const DayCounter&, const DayCounterDivisor&, RenderableGrid&> query) {
            for (auto [dayCounter, divisor, grid] : query)
            {
                auto day = (progression.daysSurvived / divisor.divisor) % 10;
                switch (day)
                {
                case 0:
                    grid.asset = dayCounter.digit0;
                    break;
                case 1:
                    grid.asset = dayCounter.digit1;
                    break;
                case 2:
                    grid.asset = dayCounter.digit2;
                    break;
                case 3:
                    grid.asset = dayCounter.digit3;
                    break;
                case 4:
                    grid.asset = dayCounter.digit4;
                    break;
                case 5:
                    grid.asset = dayCounter.digit5;
                    break;
                case 6:
                    grid.asset = dayCounter.digit6;
                    break;
                case 7:
                    grid.asset = dayCounter.digit7;
                    break;
                case 8:
                    grid.asset = dayCounter.digit8;
                    break;
                case 9:
                    grid.asset = dayCounter.digit9;
                    break;
                }
            }
        });
}
