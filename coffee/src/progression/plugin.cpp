#include "plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(coffee::Progression)
{
    return cubos::core::ecs::TypeBuilder<Progression>("coffee::Progression")
        .withField("dayDuration", &Progression::dayDuration)
        .withField("daysPassed", &Progression::daysPassed)
        .withField("timeOfDay", &Progression::timeOfDay)
        .build();
}

void coffee::progressionPlugin(Cubos& cubos)
{
    cubos.resource<Progression>();

    cubos.system("advance Progression").call([](const DeltaTime& dt, Progression& progression) {
        if (progression.timeOfDay < progression.dayDuration)
        {
            progression.timeOfDay += dt.value();
        }
        else
        {
            progression.timeOfDay = 0.0F;
            progression.daysPassed += 1;
        }
    });
}
