#include "explosion.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

CUBOS_REFLECT_IMPL(demo::Explosion)
{
    return cubos::core::ecs::TypeBuilder<Explosion>("demo::Explosion")
        .withField("duration", &Explosion::duration)
        .withField("particleTime", &Explosion::particleTime)
        .withField("particleLife", &Explosion::particleLife)
        .withField("timer", &Explosion::timer)
        .build();
}
