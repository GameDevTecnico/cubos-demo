// projectile impulse
// fire rate
// time since last shot
#pragma once

#include <cubos/core/reflection/reflect.hpp>
#include <glm/vec3.hpp>

struct Gun
{
    CUBOS_REFLECT;

    int player = 0;
    float timeSinceLastShot = 0.0F;
    float minTimeBetweenShots = 2.0F;
    glm::vec3 bulletImpulse = {0.0F, 0.0F, 60.0F};
};
