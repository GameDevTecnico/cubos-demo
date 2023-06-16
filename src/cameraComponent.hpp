#pragma once

#include <glm/glm.hpp>

struct [[cubos::component("follow", VecStorage)]] FollowEntity
{
    int idToFollow = 0;
    glm::vec3 offset = {0.0f, 15.0f, -40.0f};
};