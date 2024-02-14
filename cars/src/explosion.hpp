#pragma once

#include <cubos/engine/cubos.hpp>
#include <glm/vec3.hpp>

struct [[cubos::component("explosion")]] Explosion
{
    float life = 3.0F;
    float particleTimer = 0.0F;
};

struct [[cubos::component("particle")]] Particle
{
    float startLife = 1.0F;
    float life = 1.0F;
    float size = 1.0F;
    glm::vec3 velocity;
};

void explosionPlugin(cubos::engine::Cubos& cubos);
