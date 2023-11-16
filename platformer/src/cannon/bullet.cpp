#include "bullet.hpp"

#include <cubos/core/ecs/component/reflection.hpp>

#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

CUBOS_REFLECT_IMPL(demo::Bullet)
{
    return cubos::core::ecs::ComponentTypeBuilder<Bullet>("demo::Bullet")
        .withField("velocity", &Bullet::velocity)
        .withField("life", &Bullet::life)
        .build();
}
