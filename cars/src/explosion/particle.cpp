#include "particle.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

CUBOS_REFLECT_IMPL(demo::Particle)
{
    return cubos::core::ecs::TypeBuilder<Particle>("demo::Particle")
        .withField("startLife", &Particle::startLife)
        .withField("life", &Particle::life)
        .withField("size", &Particle::size)
        .withField("velocity", &Particle::velocity)
        .build();
}
