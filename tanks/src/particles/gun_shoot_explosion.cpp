#include "gun_shoot_explosion.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

CUBOS_REFLECT_IMPL(GunShootExplosion)
{
    return cubos::core::ecs::TypeBuilder<GunShootExplosion>("GunShootExplosion")
        .withField("duration", &GunShootExplosion::duration)
        .withField("particleTime", &GunShootExplosion::particleTime)
        .withField("particleLife", &GunShootExplosion::particleLife)
        .withField("timer", &GunShootExplosion::timer)
        .build();
}
