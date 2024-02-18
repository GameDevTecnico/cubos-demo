#include "tank.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

CUBOS_REFLECT_IMPL(Tank)
{
    return cubos::core::ecs::TypeBuilder<Tank>("Tank").withField("player", &Tank::player).build();
}

CUBOS_REFLECT_IMPL(TankTower)
{
    return cubos::core::ecs::TypeBuilder<TankTower>("TankTower").build();
}

CUBOS_REFLECT_IMPL(TankBody)
{
    return cubos::core::ecs::TypeBuilder<TankBody>("TankBody").withField("speed", &TankBody::speed).withField("angularVelocity", &TankBody::angularVelocity).build();
}