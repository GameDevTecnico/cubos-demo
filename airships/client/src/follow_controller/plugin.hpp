#pragma once

#include <cubos/engine/prelude.hpp>

#include <string>

namespace airships::client
{
    /// @brief Component which allows a player to control a follow camera, orbiting around a target entity.
    ///
    /// Assumes that the entity is the source of a @ref Follow relation.
    struct FollowController
    {
        CUBOS_REFLECT;

        /// @brief Index of the player controlling this entity. Set to -1 if no player is controlling.
        int player = -1;

        /// @brief Name of the axis used to control the camera's distance from the target.
        std::string zoomAxis = "camera-zoom";

        /// @brief Name of the axis used to control the camera's phi rotation.
        std::string phiAxis = "camera-vertical";

        /// @brief Name of the axis used to control the camera's theta rotation.
        std::string thetaAxis = "camera-horizontal";

        /// @brief Action to toggle whether the mouse should be used to control the camera.
        std::string toggleMouseAction = "camera-toggle-mouse";

        /// @brief Minimum distance from the target.
        float minZoom = 1.0F;

        /// @brief Maximum distance from the target.
        float maxZoom = 100.0F;

        /// @brief How much the camera's distance from the target can change per second.
        float zoomSpeed = 10.0F;

        /// @brief How fast should the camera rotate around the target, in degrees per second.
        float rotationSpeed = 45.0F;

        /// @brief Whether to use the mouse to control the camera.
        bool useMouse = true;

        /// @brief Mouse sensitivity.
        float mouseSensitivity = 0.1F;

        /// @brief Mouse scroll sensitivity.
        float scrollSensitivity = 1.0F;
    };

    void followControllerPlugin(cubos::engine::Cubos& cubos);
} // namespace airships::client
