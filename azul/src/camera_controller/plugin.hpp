#pragma once

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/prelude.hpp>

#include <glm/vec3.hpp>

namespace demo
{
    /// @brief Component which keeps information about the tile-map.
    ///
    /// On addition, immediately creates entities for the tile chunks.
    struct CameraController
    {
        CUBOS_REFLECT;

        /// @brief Yaw of the camera.
        float yaw = 0.0F;

        /// @brief Pitch of the camera.
        float pitch = 0.0F;

        /// @brief Distance of the camera to the center.
        float distance = 10.0F;

        /// @brief Center of the camera.
        glm::vec3 center = glm::vec3(0.0F);
    };

    void cameraControllerPlugin(cubos::engine::Cubos& cubos);
} // namespace demo
