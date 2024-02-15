#pragma once

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/prelude.hpp>

namespace demo
{
    struct Racer
    {
        CUBOS_REFLECT;

        cubos::engine::Entity currentCheckpoint;
        glm::vec3 startingPosition = {0.0F, 0.0F, 0.0F};
        glm::quat startingRotation = glm::quat(1.0F, 0.0F, 0.0F, 0.0F);
        float currentLapTime = 0.0F;
        int successiveLaps = 0;
        int maxSuccessiveLaps = 0;
        std::vector<float> lapTimes;
    };
} // namespace demo
