#include "gun.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/glm.hpp>

CUBOS_REFLECT_IMPL(Gun)
{
    return cubos::core::ecs::TypeBuilder<Gun>("Gun")
        .withField("player", &Gun::player)
        .withField("timeSinceLastShot", &Gun::timeSinceLastShot)
        .withField("minTimeBetweenShots", &Gun::minTimeBetweenShots)
        .withField("bulletImpulse", &Gun::bulletImpulse)
        .build();
}
