#pragma once

#include <glm/glm.hpp>

struct [[cubos::component("car", VecStorage)]] Car
{
    int id = 0;
    glm::vec3 vel = {0.0f, 0.0f, 0.0f};
    float angVel = 0.0f;
    float deadTime = 0.0F;
    float lapTime = -1.0F;
};
