#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <cubos/engine/cubos.hpp>

struct [[cubos::component("follow", VecStorage)]] FollowEntity
{
    cubos::core::ecs::Entity entityToFollow;
    //int idToFollow = 0;
    glm::vec3 positionOffset = {0.0f, 15.0f, -40.0f};
    glm::quat rotationOffset = glm::quat(glm::vec3(0, 0, 0));
};

void followPlugin(cubos::engine::Cubos& cubos);