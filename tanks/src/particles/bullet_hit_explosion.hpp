#pragma once

#include <cubos/core/reflection/reflect.hpp>

struct BulletHitExplosion
{
    CUBOS_REFLECT;

    // How long the explosion will last.
    float duration = 0.1F;

    // Time in seconds between spawning new particles.
    float particleTime = 0.005F;

    // Time in seconds each particle will live.
    float particleLife = 0.3F;

    // Internal timer which spawns new particles when it reaches 0.
    float timer = 0.0F;
};
