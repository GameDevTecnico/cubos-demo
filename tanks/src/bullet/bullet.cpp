#include "bullet.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

CUBOS_REFLECT_IMPL(Bullet)
{
    return cubos::core::ecs::TypeBuilder<Bullet>("Bullet")
        .withField("damage", &Bullet::damage)
        .withField("time", &Bullet::time)
        .build();
}
