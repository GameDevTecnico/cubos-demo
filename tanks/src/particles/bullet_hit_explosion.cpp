#include "bullet_hit_explosion.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

CUBOS_REFLECT_IMPL(BulletHitExplosion)
{
    return cubos::core::ecs::TypeBuilder<BulletHitExplosion>("BulletHitExplosion")
        .withField("duration", &BulletHitExplosion::duration)
        .withField("particleTime", &BulletHitExplosion::particleTime)
        .withField("particleLife", &BulletHitExplosion::particleLife)
        .withField("timer", &BulletHitExplosion::timer)
        .build();
}
