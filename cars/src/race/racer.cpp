#include "racer.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/vector.hpp>

CUBOS_REFLECT_IMPL(demo::Racer)
{
    return cubos::core::ecs::TypeBuilder<Racer>("demo::Racer")
        .withField("currentCheckpoint", &Racer::currentCheckpoint)
        .withField("startingPosition", &Racer::startingPosition)
        .withField("startingRotation", &Racer::startingRotation)
        .withField("currentLapTime", &Racer::currentLapTime)
        .withField("successiveLaps", &Racer::successiveLaps)
        .withField("maxSuccessiveLaps", &Racer::maxSuccessiveLaps)
        .withField("lapTimes", &Racer::lapTimes)
        .build();
}
