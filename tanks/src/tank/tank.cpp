#include "tank.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

CUBOS_REFLECT_IMPL(Tank)
{
    return cubos::core::ecs::TypeBuilder<Tank>("Tank")
        .withField("player", &Tank::player)
        .withField("speed", &Tank::speed)
        .withField("angularVelocity", &Tank::angularVelocity)
        .build();
}

CUBOS_REFLECT_IMPL(Turret)
{
    return cubos::core::ecs::TypeBuilder<Turret>("Turret")
        .withField("player", &Turret::player)
        .withField("rotationVelocity", &Turret::rotationVelocity)
        .build();
}
