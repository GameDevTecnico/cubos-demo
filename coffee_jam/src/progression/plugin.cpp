#include "plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(demo::Progression)
{
    return cubos::core::ecs::TypeBuilder<Progression>("demo::Progression")
        .withField("dayDuration", &Progression::dayDuration)
        .withField("daysSurvived", &Progression::daysSurvived)
        .withField("timeOfDay", &Progression::timeOfDay)
        .withField("scoreToFinishNight", &Progression::scoreToFinishNight)
        .withField("score", &Progression::score)
        .build();
}

void demo::progressionPlugin(Cubos& cubos)
{
    cubos.resource<Progression>();

    cubos.system("advance Progression").call([](const DeltaTime& dt, Progression& progression) {
        if (progression.timeOfDay < progression.dayDuration)
        {
            progression.timeOfDay += dt.value();
        }
        else if (progression.scoreToFinishNight != -1 && progression.score >= progression.scoreToFinishNight)
        {
            progression.timeOfDay = 0.0F;
            progression.daysSurvived += 1;
            progression.scoreToFinishNight = -1;
            progression.score = 0;
        }
    });
}
